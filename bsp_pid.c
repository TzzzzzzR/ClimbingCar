#include "bsp_pid.h"
#include <stdio.h>
#include "usart.h"

_pid_loc pidl;//����ȫ�ֱ������������ֱ����_pid_loc�ᱨ����ʱ��֪��Ϊʲô��
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
	
/*output(ʵ�����) = Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]*/
float pidl_realize(float target, float temp_val)
{
	pidl.target_val = target;			//����Ŀ��ֵ
	pidl.err = pidl.target_val-temp_val;//�������
	pidl.integral += pidl.err;	//λ��ʽ�����(�����ۼ�)
	pidl.actual_val = pidl.Kp*pidl.err + pidl.Ki*pidl.integral + pidl.Kd*(pidl.err - pidl.last_err);
	pidl.err = pidl.last_err;	//���ݣ�������һ�μ��㣩
	return pidl.actual_val;		//�������ֵ
}	


/*output += Kp[e��k��-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)������ƫ��,e(k-1)������һ�ε�ƫ��  �Դ�����*/
float pidi_realize(float target, float temp_val)
{
	pidi.target_val = target;	
	pidi.err = pidi.target_val - temp_val;
	float increment_val = pidi.Kp*(pidi.err - pidi.last_err) + pidi.Ki*pidi.err + pidi.Kd*(pidi.err - 2*pidi.last_err + pidi.prev_err);
	pidi.actual_val += increment_val;	//���������������Ҫ+=
	pidi.prev_err = pidi.last_err;		//����
	pidi.last_err = pidi.err;
	return pidi.actual_val;
}

//pid���ܺ�����Ϊ��ʵ�ֶ�ʱ����pid����->���ж��м���pid,����pid�ṹ������������
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

