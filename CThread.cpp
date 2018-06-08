#include "HttpServer.h"

vector<CThread *> *CThread::m_listRunningThreads=NULL;


void CThread::eraseThreadFromList(CThread *th){
	int index = getIndexThread(th);

	if( index != -1){
		m_listRunningThreads->erase(m_listRunningThreads->begin()+index);
	}
	else{
		fprintf(stderr,"thread not exist on list\n");
	}

}

void CThread::addThreadToList(CThread *th){
	int index = getIndexThread(th);

	if( index == -1){
		m_listRunningThreads->push_back(th);
	}
	else{
		fprintf(stderr,"thread already exist on list\n");
	}
}

void CThread::pauseAllThreads(){
	for(unsigned i = 0 ; i < m_listRunningThreads->size(); i++){
		m_listRunningThreads->at(i)->pause();
	}

}

void CThread::resumeAllThreads(){
	for(unsigned i = 0 ; i < m_listRunningThreads->size(); i++){
		m_listRunningThreads->at(i)->resume();
	}

}


int CThread::getIndexThread(CThread *th){
	if(m_listRunningThreads!=NULL){
		for(unsigned i = 0; i<m_listRunningThreads->size(); i++){
			if(m_listRunningThreads->at(i) == th) return i;
		}
	}
	return -1;
}

void CThread::createSingletons(){
	if(m_listRunningThreads == NULL)
		m_listRunningThreads = new vector<CThread *>;
}

void CThread::destroySingletons(){
	if(m_listRunningThreads != NULL){
		for(unsigned i = 0; i < m_listRunningThreads->size(); i++){
			 m_listRunningThreads->at(i)->stop();
		}

		delete m_listRunningThreads;
	}

	m_listRunningThreads = NULL;
}


//---------------------------------- SDL Thread proxy based on C++ function --------------------
int CThread::_thread_runner(void* data) {
	return reinterpret_cast<CThread*>(data)->run();
}


CThread::CThread()
{
    thread = NULL;
    end_loop_mdb = true;
    long number = (long)this;
    name_thread= "name_thread_"+CStringUtils::intToString(number);
    time_delay=16;
    m_pausable=true;
    m_paused=false;
}


bool CThread::isPausable(){
	return m_pausable;
}

void CThread::setPausable(bool _pausable){
	m_pausable = _pausable;
}

void CThread::pause(){
	if(m_pausable)
		m_paused = true;
}

void CThread::resume(){
	m_paused = false;
}

bool CThread::isRunning(){
	return (m_paused	|| (end_loop_mdb) || (thread == NULL));
}

//-------------------------------------------------------------------------------------
void CThread::start()
{
	// thread is running ...
	if(thread)
		return;

    end_loop_mdb = false;

   	addThreadToList(this);

    printf("---- starting thread %s ----\n", name_thread.c_str());

    thread = SDL_CreateThread(_thread_runner, name_thread.c_str(), this);

   // SDL_Delay(10);

	//thread = SDL_CreateThread((int (*)(void *))internalThreadUpdate_C,this);// CThread::internalThreadUpdate);//SDL_CreateThread(FunctionThreadUpdate,CThread::FunctionThreadUpdate);
    
}
//-------------------------------------------------------------------------------------
void CThread::setTimeDelay(Uint32 delay){
	time_delay = delay;
}
//-------------------------------------------------------------------------------------
int CThread::run()
{
    while(!end_loop_mdb)
    {
    	if(!m_paused){
    		mainLoopThread();
    	}

    	if(time_delay > 0){
    		SDL_Delay(time_delay);
    	}

    }

	return 0;
}
//-------------------------------------------------------------------------------------
void CThread::stop()
{
    if(!end_loop_mdb && thread != NULL)
    {
		printf("---- shutingdown thread %s ---\n", name_thread.c_str());

		end_loop_mdb = true;

		SDL_WaitThread(thread, NULL);

		//SDL_Delay(10);
		eraseThreadFromList(this);

		thread = NULL;
    }
}

CThread::~CThread()
{
    stop();
}
