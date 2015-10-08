#ifndef __ASYNC_VBO_TRANSFERS_UTILS_H__
#define __ASYNC_VBO_TRANSFERS_UTILS_H__

#include <cstdlib>
#include <ctime>

namespace util{

static int rndFromInterval(int min, int max){
	int result = rand();
	result %= (max - min);
	return result + min;
}

}

#endif