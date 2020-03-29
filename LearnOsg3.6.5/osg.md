
# 阅读列表：

- Referenced：osg内存管理对象，实现智能指针的引用计数和释放
  1. 全局线程安全： getGlobalReferencedMutex()
  2. 获取所有引用此对象的观察者集合： getObserverSet()
  3. 减少应用而不删除对象内存：unref_nodelete， 例如调用release(),返回裸指针
  4. 设置全局DeleteHandler对象指针，用户自定义删除操作：setDeleteHandler()
  5. deleteUsingDeleteHandler(): 使用DeleteHandler处理对象的删除工作。
  6. signalObserversAndDelete(bool signalDelete, bool doDelete)： 处理对象删除策略

- DeleteHandler： 删除句柄，用于Referenced的释放回调
  1. Deletehandler没有继承子Referenced，可以使用std::auto_ptr进行管理，但实现上使用了：ResetPointer对其管理。
  2. DeleteHandler可以设置删除物体保留n帧后删除：setNumFramesToRetainObjects

- ref_ptr： osg内置的智能指针实现
  1.  OSG_USE_REF_PTR_IMPLICIT_OUTPUT_CONVERSION： 实现智能指针到裸指针的自动转行
        operator T*() const { return _ptr; }
  2. 智能指针，处理了bool判断： safe bool idiom

- AlphaFunc
