#pragma once

class CThread{

	string name_thread;
	int time_delay;
	bool m_pausable;
	bool m_paused;
	static int _thread_runner(void *data);

	static vector<CThread *> *m_listRunningThreads;

	static void eraseThreadFromList(CThread *th);
	static void addThreadToList(CThread *th);
	static int getIndexThread(CThread *m);

protected:
    SDL_Thread *thread;
    bool end_loop_mdb;
	virtual void mainLoopThread() =0;
	int run();

	void setTimeDelay(Uint32 delay);
public:


	static void createSingletons();
	static void destroySingletons();

	static void pauseAllThreads();
	static void resumeAllThreads();

    CThread();

    bool isPausable();
    bool isRunning();
    void setPausable(bool m_pausable);

    virtual void start();
    virtual void resume();
    virtual void pause();
    virtual void stop();
    virtual ~CThread();
};


