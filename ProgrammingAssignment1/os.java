class Stack
{
    private Object[] stack;
    private int head;
    private int max;

    private final Lock lock = new ReentrantLock();
    private final Condition isEmpty = lock.newCondition();
    private final Condition isFull = lock.newCondition();

    public Stack(int max)
    {
        stack = new Object[max];
        head = -1;
    }

    public boolean push(Object node)
    {
        lock.lock();
        while (stack.length() <= max)
        {
            isFull.await();
            //push until full
        }
    }

    public Object pop()
    {
        synchronized(this)
        {
            lock.lock();
            while (head != NULL)
            {
                isEmpty.await();
            }
            //pop all nodes
        }
    }
}