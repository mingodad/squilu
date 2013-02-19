/* -*-C++-*- 

   "$Id: FBase.H,v 1.3 2000/05/14 06:05:53 jamespalmer Exp $"
   
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

#ifndef _FBASE_H_
#define _FBASE_H_

/** @package libflek_core
 * FBase is the Abstract base class for all f classes.  The FBase
 * class can be used to build container classes which use object 
 * pointers.  The same container class can then be used with any
 * derived classes without the need of any additional code or templates.
 *
 * FBase has no member data.  It only has protected constructors (to 
 * prevent instantiation), virtual destructors and an assignment operator.
 */
class FBase
{
  
 public:

  typedef FBase* Ptr;

  /**
   * Assignment operator.  This operator must be overriden to be useful,
   * it's default behavior is to do nothing.
   * 
   * @param src
   */
  FBase& operator = (const FBase&) {
    return *this;
  }

  /**
   * Destructor.
   */
  virtual ~FBase () {}

  /**
   * Derived class should give a meaningful implementation
   * for the following functions.
   *
   * Classes such as List which use FBase pointers
   * will use these functions, for memory management
   * 
   * Make a copy of the FBase and return a pointer to the new one.
   */
  virtual Ptr copy () const = 0;

protected:
  
  /**
   * The default constructor is protected to prevent instantiation,
   */
  FBase () {}

  /** 
   * The copy constructor is protected to prevent instantiation.
   * 
   * @param src The class to take initial data from.
   */
  FBase (const FBase&) {}
};

#endif // #ifndef FBASE_H_

