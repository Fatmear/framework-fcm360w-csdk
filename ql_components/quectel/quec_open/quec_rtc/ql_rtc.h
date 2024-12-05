#ifndef __QL_RTC_H_
#define __QL_RTC_H_


/**
 * @brief DATE_RTC struct.
 */
typedef struct{
	unsigned short 	year;
	unsigned char 	month;
	unsigned char  	day;
	unsigned char  	hour;
	unsigned char  	min;
	unsigned char  	sec;
	unsigned char 	wday;
}QL_DATE_RTC;

/**    @brief		set hal time function .
*	   @param[in]	rtc_time    Total seconds since January 1, 1970
*	   @return  	0-- set hal rtc time succeed, -1--set time failed
*/
int ql_rtc_set_time(int rtc_time);

/**    @brief		get hal time function .
*	   @return  	Total seconds since January 1, 1970
*/
int ql_rtc_get_time();

/**    @brief		format convert. utctime to DATE_RTC struct convert
*	   @param[in]	utc_time    Total seconds since January 1, 1970
*	   @param[in]	date    	point of DATE_RTC struct data;
*	   @return  	0-- succeed 	-1-- failed
*/
int ql_rtc_utctime_to_date(int utc_time, QL_DATE_RTC * date);

/**    @brief		format convert. utctime to DATE_RTC struct convert
*	   @param[in]	date    	DATE_RTC struct data;
*	   @param[out]	utc_time    the point of Total seconds since January 1, 1970
*	   @return  	0-- succeed 	-1-- failed
*/
int ql_rtc_date_to_utctime(QL_DATE_RTC date, int * utc_time);

int ql_rtc_init();

unsigned char ql_rtc_get_weekday(int year, int month, int day);
#endif