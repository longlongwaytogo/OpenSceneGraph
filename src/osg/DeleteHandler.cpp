/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/
#include <osg/DeleteHandler>
#include <osg/Notify>

namespace osg
{

DeleteHandler::DeleteHandler(int numberOfFramesToRetainObjects):
    _numFramesToRetainObjects(numberOfFramesToRetainObjects),
    _currentFrameNumber(0)
{
}

DeleteHandler::~DeleteHandler()
{
    // flushAll();
}

void DeleteHandler::flush()
{
    typedef std::list<const osg::Referenced*> DeletionList;
    DeletionList deletionList;

    {
        // gather all the objects to delete whilst holding the mutex to the _objectsToDelete
        // list, but delete the objects outside this scoped lock so that if any objects deleted
        // unref their children then no deadlock happens.
        // 收集所有要删除的对象，同时将互斥锁保存在_objectsToDelete 
        // 列表中，但是删除此作用域锁之外的对象，这样，如果删除了任何对象
        // 取消引用其子对象，则不会发生死锁。
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        unsigned int frameNumberToClearTo = _currentFrameNumber - _numFramesToRetainObjects;

        ObjectsToDeleteList::iterator itr;
        for(itr = _objectsToDelete.begin();
            itr != _objectsToDelete.end();
            ++itr)
        {
            if (itr->first > frameNumberToClearTo) break;

            deletionList.push_back(itr->second);

            itr->second = 0;
        }

        _objectsToDelete.erase( _objectsToDelete.begin(), itr);
    }

    for(DeletionList::iterator ditr = deletionList.begin();
        ditr != deletionList.end();
        ++ditr)
    {
        doDelete(*ditr);
    }
   // 为什么不直接遍历_objectsToDete并删除对象呢？ 因为删除操作会使迭代器失效，
   // 所以，先收集，再删除，同时在最开始进行加锁保护。
}

void DeleteHandler::flushAll()
{
    unsigned int temp_numFramesToRetainObjects = _numFramesToRetainObjects;
    _numFramesToRetainObjects = 0; 
    // _numFramesToRetainObjects=0,保证执行flashAll过程中，
    // 对_objectsToDete列表的不会进行添加，而直接执行删除操作。

    typedef std::list<const osg::Referenced*> DeletionList;
    DeletionList deletionList;

    {
        // gather all the objects to delete whilst holding the mutex to the _objectsToDelete
        // list, but delete the objects outside this scoped lock so that if any objects deleted
        // unref their children then no deadlock happens.
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        ObjectsToDeleteList::iterator itr;
        for(itr = _objectsToDelete.begin();
            itr != _objectsToDelete.end();
            ++itr)
        {
            deletionList.push_back(itr->second);
            itr->second = 0;
        }

        _objectsToDelete.erase( _objectsToDelete.begin(), _objectsToDelete.end());
    }

    for(DeletionList::iterator ditr = deletionList.begin();
        ditr != deletionList.end();
        ++ditr)
    {
        doDelete(*ditr);
    }

    _numFramesToRetainObjects = temp_numFramesToRetainObjects;
}

void DeleteHandler::requestDelete(const osg::Referenced* object)
{
    if (_numFramesToRetainObjects==0) doDelete(object);
    else
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        _objectsToDelete.push_back(FrameNumberObjectPair(_currentFrameNumber,object));
    }
}

} // end of namespace osg
