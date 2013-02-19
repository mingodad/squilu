/* -*-C++-*-

   "$Id: FDate.H,v 1.4 2000/09/29 21:49:35 clip Exp $"

   Copyright 1999-2000 by the Flek development team.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.

   Please report all bugs and problems to "flek-devel@sourceforge.net".

*/

#ifndef __FDATE_H__
#define __FDATE_H__

#include <FLU/FBase.H>
// CET - libstdc++ ios evil on Linux
//#include <iostream.h>

typedef enum {
  SUNDAY,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
} FWeekday;

typedef enum {
  JANUARY = 1,
  FEBRUARY,
  MARCH,
  APRIL,
  MAY,
  JUNE,
  JULY,
  AUGUST,
  SEPTEMBER,
  OCTOBER,
  NOVEMBER,
  DECEMBER
} FMonth;

/** @package libflek_core
 * FDate provides a date class that stores the day, month and year.
 */
class FDate : public FBase
{

 public:

  /**
   * Default constructor.
   */
  FDate ();

  /**
   * Three argument constructor.  Initializes this object with the
   * year, month and day.
   *
   * @param y The year.
   * @param m The month.
   * @param d The day.
   */
  FDate (int y, int m, int d);

  /**
   * Copy constructor.
   *
   * @param src The FDate object to copy initial values from.
   */
  FDate (const FDate& src);

  /* (ScanDoc doesn't like this one)
   * Copy method.  This virtual method makes a new copy of this object.
   *
   * @return The pointer (FBase::Ptr) to the new FDate instance.
   */
  virtual FBase::Ptr copy () const;

  /**
   * Sets the date from the year, month, and day.
   *
   * @param y The year.
   * @param m The month, 1 through 12.
   * @param d The day, 1 through the numbers of days in that month.
   */
  void set_date (int y, int m, int d);

  /**
   * Sets the date value from another FDate object.
   *
   * @param src The FDate object to set values from.
   */
  void set_date (const FDate& src);

  /**
   * Sets the date format.
   *
   * @param fmt The date format.
   */
  void set_format (int fmt);

  /**
   * Set the date value to today's date.
   */
  void today ();

  /**
   * Sets the year.
   *
   * @param y The year to associate with this object.
   */
  void year (int);

  /**
   * Gets the year.
   *
   * @return The year associated with this object.
   */
  int year ();

  /**
   * Sets the month.
   *
   * @param m The month to associate with this object.
   */
  void month (int m);

  /**
   * Gets the month.
   *
   * @return The month associated with this object.
   */
  int  month ();

  /**
   * Sets the day.
   *
   * @param d The day of the month to associate with this object.
   */
  void day (int d);

  /**
   * Gets the day.
   *
   * @return The day of the month associated with this object.
   */
  int day ();

  /**
   * Gets the Julian date.
   *
   * @return The julian date value for this object.
   */
  double julian_date ();

  /**
   * Equality comparison of two dates.
   *
   * @param src The FDate object to compare this one to.
   * @return True if this object and src are equal.  False otherwise.
   */
  bool operator== (const FDate& src);

  /**
   * Inequality comparison of two dates.
   *
   * @param src The FDate object to compare this one to.
   * @return True if this object and src are <b>not</b> equal.  False otherwise.
   */
  bool operator!= (const FDate& src);

  /**
   * Comparison of two dates.
   *
   * @param src The FDate object to compare this one to.
   * @return True if this FDate object is less than the src object.  False otherwise.
   */
  bool operator< (const FDate& src);

  /**
   * Comparison of two dates.
   *
   * @param src The FDate object to compare this one to.
   * @return True if this FDate object is greater than the src object.  False otherwise.
   */
  bool operator> (const FDate& src);

  /**
   * Sets one date equal to another date.
   *
   * @param src The FDate object to set values from.
   */
  void operator= (const FDate& src);

  /**
   * Adds days to the date.
   *
   * @param d The number of days to add to this date.
   */
  const FDate &operator+= (int d);

  /**
   * Pre-increment operator.  Increment the date by one day.
   */
  FDate &operator++ ();

  /**
   * Post-increment operator. Increment the date by one day.
   */
  FDate operator++ (int);

  /**
   * ostream operator.
   */
  // CET - libstdc++ ios evil on Linux
  // friend ostream& operator << (ostream &, const FDate &);

  /**
   * Gets wether day d is at the end of the month.
   *
   * @param d The day to check.
   * @return True if d is at the end of the month.  False otherwise.
   */
  bool end_of_month (int d);

  /**
   * Gets wether year y is a leap year.
   *
   * @param y The year to check.
   * @return True if the year is a leap year.  False otherwise.
   */
  static bool leap_year (int y);

  /**
   * Gets wether the year associated with this object is a leap year.
   *
   * @return True if the year is a leap year.  False otherwise.
   */
  bool leap_year ()
    { return leap_year (Year); }

  /**
   * Gets wether the passed date is valid.
   *
   * @param y The year.
   * @param m The month, 1 through 12.
   * @param d The day, 1 through the numbers of days in that month.
   * @return True if the year, month and day make a valid date.  False otherwise.
   */
  static bool valid (int y, int m, int d);

  /**
   * Gets wether the current date is valid.
   *
   * @return True if the year, month and day make a valid date.  False otherwise.
   */
  bool valid ()
    { return valid (Year, Month, Day); }

  /**
   * Gets the number of days in the month for a given month and leap value.
   *
   * @param m The month, 1 through 12.
   * @param leap Should be 1 if the associated year is a leap year, 0 otherwise.
   * @return The number of days in the month.
   */
  static int days_in_month (int m, int leap);

  /**
   * Gets the number of days in the month for this date.
   *
   * @return The number of days in the month.
   */
  int days_in_month ()
    { return days_in_month (Month, leap_year (Year)); }

  /**
   * Gets the day of the year for the passed date.
   *
   * @param y The year.
   * @param m The month, 1 through 12.
   * @param d The day, 1 through the numbers of days in that month.
   * @return The day of the year.
   */
  static int day_of_year (int y, int m, int d);

  /**
   * Gets the day of the year for this date.
   *
   * @return The day of the year.
   */
  int day_of_year ()
    { return day_of_year (Year, Month, Day); }

  /**
   * Gets the day of the epoch for the passed date.
   *
   * @param y The year.
   * @param m The month, 1 through 12.
   * @param d The day, 1 through the numbers of days in that month.
   * @return The day of the epoch.
   */
  static int day_of_epoch (int y, int m, int d);

  /**
   * Gets the day of the epoch for this date.
   *
   * @return The day of the epoch.
   */
  int day_of_epoch ()
    { return day_of_epoch (Year, Month, Day); }

  /**
   * Gets the day of the week for the passed date.
   *
   * @param y The year.
   * @param m The month, 1 through 12.
   * @param d The day, 1 through the numbers of days in that month.
   * @return The day of the week, 1 through 7.
   */
  static int day_of_week (int y, int m, int d);

  /**
   * Gets the day of the week for this date.
   *
   * @return The day of the week, 1 through 7.
   */
  int day_of_week ()
    { return day_of_week (Year, Month, Day); }

  /**
   * Decrement the date by one month.
   */
  void previous_month ();

  /**
   * Increment the date by one month.
   */
  void next_month ();

  /**
   * Decrement the date by one year.
   */
  void previous_year ();

  /**
   * Increment the date by one year.
   */
  void next_year ();

  /**
   * Convert the date to a string.
   *
   * @param fmt The format to use during conversion.
   */
  char* to_string (int fmt) const;

  /**
   * Convert the date to a string using the current format.
   */
  char* to_string () const;

  /**
   * An array of days of the month for a non leap year, 1-12. (0 is ignored.)
   */
  static const int days[];

  /**
   * An array of total days since the beginning of the year for
   * each month, 1-12.  (0 is ignored.)
   */
  static const int julian_days[2][13];

  /**
   * An array of month names.
   */
  static const char* month_name[];

private:

  int  Year;
  int  Month;
  int  Day;
  int  Fmt;
  void help_increment ();

};

#endif
