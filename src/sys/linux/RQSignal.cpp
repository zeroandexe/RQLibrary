#include "sys/linux/RQSignal.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <errno.h>

RQSignal::RQSignal(unsigned int startCount)
{
	sem_init(&_semId, 0, startCount);
}

RQSignal::~RQSignal()
{
	sem_destroy(&_semId);
}

void RQSignal::Add()
{
	sem_post(&_semId);
}

void RQSignal::Sub()
{
	sem_wait(&_semId);
}

void RQSignal::Zero()
{
	sem_init(&_semId, 0, 0);
}

