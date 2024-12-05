#include "ql_api_osi.h"
#include "ql_debug.h"
#include "ql_rtc.h"
#include "cli.h"
#define QL_ONE_HOUR2SEC 3600
#define  QL_IS_LEAP_YEAR(year) (year%400==0 || (year%4==0&&year%100!=0))
// #define	FEBRUARY_OF_LEAPEYEAR	29
const char month_days2[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// extern unsigned char rtc_get_weekday(int year, int month, int day);
// #define DIFF_SEC_1970_2036          ((unsigned int)2085978496L)
const char *ql_weeken[7] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
#ifdef CONFIG_SNTP
#include "sntp_tr.h"
#endif
static int ql_cli_rtc_set_time(cmd_tbl_t *t, int argc, char *argv[])
{
	int time;

	if (argc >= 2)
	{
		time = (int)strtoul(argv[1], NULL, 0);
	}
	else
	{
		os_printf(LM_CMD,LL_INFO,"\r\nunit test rtc, err: no enough argt!\r\n");
		return 0;
	}
#ifdef CONFIG_SNTP
	float tz = get_timezone();
	int local_time = time + (int)(tz * 3600);
	if (ql_rtc_set_time(local_time) == 0)
#else
	if (ql_rtc_set_time(time) == 0)
#endif
	{
		os_printf(LM_CMD,LL_INFO,"\r\n ql test rtc, set rtc time = %d(s)!\r\n", time);
	}
	else
	{
		os_printf(LM_CMD,LL_INFO,"\r\n ql test rtc, set rtc failed!\r\n");
	}

	return 0;
}

CLI_SUBCMD(ql_rtc, set, ql_cli_rtc_set_time, "ql test set time", "ut_rtc set [time-seconds]");


static int ql_cli_rtc_get_time(cmd_tbl_t *t, int argc, char *argv[])
{
	int res = 0;

	res = ql_rtc_get_time();
	#ifdef CONFIG_SNTP
	float tz = get_timezone();
	res = res - (int)(tz * QL_ONE_HOUR2SEC);
	#endif
	os_printf(LM_CMD,LL_INFO,"\r\n ql test rtc, get rtc time = %d(s)!\r\n", res);

	return 0;
}

CLI_SUBCMD(ql_rtc, get, ql_cli_rtc_get_time, "unit test get time", "ut_rtc get");


static int ql_cli_rtc_utctime_to_date(cmd_tbl_t *t, int argc, char *argv[])
{
	int time;
	QL_DATE_RTC dr;
	if (argc >= 2)
	{
		time = (int)strtoul(argv[1], NULL, 0);
	}
	else
	{
		os_printf(LM_CMD,LL_INFO,"\r\n ql test rtc, err: no enough argt!\r\n");
		return 0;
	}

	if (ql_rtc_utctime_to_date(time,&dr) == 0)
	{
		os_printf(LM_CMD,LL_INFO,"\r\n ql test rtc,[%d] utctime to date: YYYY-MM-DD HH:mm:ss = %4d-%02d-%02d %02d:%02d:%02d week=%s!\r\n", time, dr.year,dr.month,dr.day,dr.hour,dr.min,dr.sec,ql_weeken[dr.wday]);
	}
	else
	{
		os_printf(LM_CMD,LL_INFO,"\r\n ql test rtc, set rtc failed!\r\n");
	}

	return 0;
}

CLI_SUBCMD(ql_rtc, utctime_to_date, ql_cli_rtc_utctime_to_date, "ql test utctime to date format", "ut_rtc utctime_to_date [time-seconds]");	

static int ql_cli_rtc_date_to_utctime(cmd_tbl_t *t, int argc, char *argv[])
{
	int time;
	unsigned char daysofmonth=0;
	unsigned char wk = 0;
	QL_DATE_RTC dr;
	if (argc >= 8)
	{
		//dr.year = (unsigned int)strtoul(argv[1], NULL, 0);
		dr.year = (unsigned short)atoi(argv[1]);
		dr.month = (unsigned char)atoi(argv[2]);
		dr.day = (unsigned char)atoi(argv[3]);
		dr.hour = (unsigned char)atoi(argv[4]);
		dr.min = (unsigned char)atoi(argv[5]);
		dr.sec = (unsigned char)atoi(argv[6]);
		dr.wday = (unsigned char)atoi(argv[7]);

		if (!(QL_IS_LEAP_YEAR(dr.year) && (dr.month == 2)))
		{
			daysofmonth = month_days2[dr.month];
		}
		else
		{
			daysofmonth = 29;
		}
		wk = ql_rtc_get_weekday(dr.year,dr.month,dr.day);
		if(	dr.year < 1970	|| dr.month > 12|| dr.hour >=24 	||\
			dr.min >=60		|| dr.sec >=60 	|| dr.wday != wk 	||\
			dr.day > daysofmonth)
		{
			os_printf(LM_CMD,LL_INFO,"\r\n ql test rtc, err: please input correct args!\r\n");
			return 0;
		}

	}
	else
	{
		os_printf(LM_CMD,LL_INFO,"\r\nql test rtc, err: no enough arg!\r\n");
		return 0;
	}

	if (ql_rtc_date_to_utctime(dr,&time) == 0)
	{
		os_printf(LM_CMD,LL_INFO,"\r\nql test rtc, YYYY-MM-DD HH:mm:ss = %4d-%02d-%02d %02d:%02d:%02d week=%s!date to utctime =[%d]\r\n", dr.year,dr.month,dr.day,dr.hour,dr.min,dr.sec,ql_weeken[dr.wday], time);
	}
	else
	{
		os_printf(LM_CMD,LL_INFO,"\r\nql test rtc, set rtc failed!\r\n");
	}

	return 0;
}

CLI_SUBCMD(ut_rtc, date_to_utctime, ql_cli_rtc_date_to_utctime, "ql date to utctime format", "ql_rtc date_to_utctime [year] [month] [day] [hour] [min] [sec] [week]");	


CLI_CMD(ql_rtc, NULL, "ql test hal rtc", "ql_hal_rtc");

