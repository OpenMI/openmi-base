#ifndef OPENMI_BASE_DATE_H_
#define OPENMI_BASE_DATE_H_ 

#include <string.h>
#include <stdio.h>
#include <sys/time.h>

namespace openmi {

#define BJTIME 8
#define DAY 86400  // 60*60*24
#define YEARFIRST 2001
#define YEARSTART (365*(YEARFIRST - 1970) + 8)
#define YEAR400 164097  // (365*4*100 + (4*(100/4 - 1) + 1))
#define YEAR100 36524   // (365*100 + (100/4 - 1))
#define YEAR004 1461    // (365*4 + 1)
#define YEAR001 365 

struct DateInfo {
  DateInfo(): year(0), month(0), day(0), hour(-1), minute(-1), second(0), usecond(0) {}
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  int usecond;
}; // struct Date

class Date {
public:
  static void HumanDate(DateInfo& date) {
    struct timeval tv;
    memset(&tv, 0, sizeof(timeval));
    gettimeofday(&tv, NULL);

    int sec = 0;
    sec = tv.tv_sec;
    // TODO timezone fetched by machine
    sec = sec + 3600*BJTIME;
    date.usecond = tv.tv_usec;

    if (sec % DAY == 0 || date.year == 0) {
      printf("sec // DAY: %d, date.year: %d\n", sec % DAY, date.year);
      ComputeYearMonthDay(sec, date);
    }

    if (sec % 3600 == 0 || date.hour == -1) {
      ComputeHour(sec, date);
    }

    if (sec % 60 == 0 || date.minute == -1) {
      ComputeMinute(sec, date);
    }

    ComputeSecond(sec, date);
  }

private:
  static void ComputeYearMonthDay(int sec, DateInfo& date) {
    printf("ComputeYearMonthDay\n");
    int m[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int ad = sec / DAY;
    ad = ad - YEARSTART;
  
    int y400 = ad / YEAR400;
    int y100 = (ad - y400 * YEAR400) / YEAR100;
    int y004 = (ad - y400 * YEAR400 - y100 * YEAR100) / YEAR004;
    int y001 = (ad - y400 * YEAR400 - y100 * YEAR100 - y004 * YEAR004) / YEAR001;

    int yy = y400*4*100 + y100*100 + y004*4 + y001*1 + YEARFIRST;
    date.year = yy;
    
    int dd = (ad - y400 * YEAR400 - y100 * YEAR100 - y004 * YEAR004) % YEAR001;
    // year 
    if (yy % 1000 == 0) {
      if ((yy/1000) % 4 == 0) {
        m[1] = 29;
      }
    } else {
      if (yy % 4 == 0) {
        m[1] = 29;
      }
    }

    // month and day
    int i;
    for (i = 1; i < 12; i++) {
      if (dd - m[i] < 0) {
        break;
      } else {
        dd = dd - m[i];
      }
    }
    date.month = i;
    date.day = dd + 1;
  }

  static void ComputeHour(int sec, DateInfo& date) {
    //printf("ComputeHour\n");
    date.hour = sec / 3600 % 24;
  }

  static void ComputeMinute(int sec, DateInfo& date) {
    //printf("ComputeMinute\n");
    date.minute = sec / 60 - sec / 3600 * 60;
  }

  static void ComputeSecond(int sec, DateInfo& date) {
    date.second = (sec - sec / 3600 * 3600) % 60;
  }
}; // class Date

} // namespace openmi
#endif // OPENMI_BASE_DATE_H_ 
