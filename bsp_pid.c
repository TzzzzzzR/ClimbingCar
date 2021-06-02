#include "bsp_pid.h"
#include <stdio.h>
#include "usart.h"

_pid_loc pidl;//定义全局变量，如果下面直接用_pid_loc会报错（暂时不知道为什么）
_pid_inc pidi;

void pid_loc_init(float p, float i, float d)
{
	pidl.target_val = 0.0;
	pidl.actual_val = 0.0;
	pidl.err = 0.0;
	pidl.last_err = 0.0;
	pidl.integral = 0.0;
	pidl.Kp = p;
	pidl.Ki = i;
	pidl.Kd = d;
	printf("pid_loc_init ok");
}	

void pid_inc_init(float p, float i, float d)
{
	pidi.target_val = 0.0;				
	pidi.actual_val = 0.0;					
	pidi.err = 0.0;								
	pidi.last_err = 0.0;					
	pidi.prev_err = 0.0;					
	pidi.Kp = p;
	pidi.Ki = i;
	pidi.Kd = d;
}
	
/*output(实际输出) = Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]*/
float pidl_realize(float target, float temp_val)
{
	pidl.target_val = target;			//建立目标值
	pidl.err = pidl.target_val-temp_val;//计算误差
	pidl.integral += pidl.err;	//位置式有求和(误差的累计)
	pidl.actual_val = pidl.Kp*pidl.err + pidl.Ki*pidl.integral + pidl.Kd*(pidl.err - pidl.last_err);
	pidl.err = pidl.last_err;	//传递（用于下一次计算）
	return pidl.actual_val;		//返回输出值
}	


/*output += Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差,e(k-1)代表上一次的偏差  以此类推*/
float pidi_realize(float target, float temp_val)
{
	pidi.target_val = target;	
	pidi.err = pidi.target_val - temp_val;
	float increment_val = pidi.Kp*(pidi.err - pidi.last_err) + pidi.Ki*pidi.err + pidi.Kd*(pidi.err - 2*pidi.last_err + pidi.prev_err);
	pidi.actual_val += increment_val;	//算出来的是增量，要+=
	pidi.prev_err = pidi.last_err;		//传递
	pidi.last_err = pidi.err;
	return pidi.actual_val;
}

//pid功能函数，为了实现定时计算pid功能->在中断中计算pid,由于pid结构体作用域问题
void pid_func()
{
	float target = 1000;
	pidl_realize(target, pidl.actual_val);
	//printf("pidl actual_val:%f\r\n", pidl.actual_val);
	printf("pidl:%f\n", pidl.actual_val);

	/*pidi_realize(target, pidi.actual_val);
	printf("pidi actual_val:%f\r\n", pidi.actual_val);	
	*/
}

