ArduinoExample
==============

Minimal eclipse project for arduino

If you want to use ArduinoCore and ArduinoLibs projects to your own projects, first, you must
import ArduinoCore project in your workspace, and optionnaly ArduinoLibs and ArduinoTools.

Then, you can clone this project, or follow these instructions :

* Create new project
  * -> C++ project
  * -> AVR Cross Target Application / Static Library
  * -> Empty Project
  * Next
    keep only Release target (will be modified later)
  * Target hardware :
    * Uno => ATmega328P / 16MHz
    * Mega => ATmega 2560 / 16MHz
    * Micro => ATmega32u4 / 16MHz
  *finish.

Then,
*Project -> edit properties :
  *  C/C++ Build -> select "Enable individual settings"
    *     Manage configuration -> rename : change name to target hardware Uno, Mega, ...
     This name must match the one in ArduinoCore project, to make eclipse link
     with the correct version.
  *  AVR -> AVRDude -> select Programmer configuration
    if it doesn't exist, create it and specify com port
  *  C/C++ build -> Settings -> panel "tool settings" :
  *    AVR Compiler -> Directories :
    *      Include Path -> "+" -> workspace -> ArduinoCore
      if you need them, do the same with ArduinoLibs and ArduinoTools
  *    AVR Compiler -> Symbols :
    *      Define syms -> "+" -> ARDUINO=105
        (not really sure it's usefull ?)
  *    AVR C++ Compiler : do preceding 2 steps here also
  *    AVR C++ Linker Libraries :
    *      Libraries (-l) : add AduinoCore, and ArduinoLibs, ArduinoTools if you need them
    *      Library Path (-L) : add ${workspace_loc:/ArduinoCore/${ConfigName}}
        and ${workspace_loc:/ArduinoLibs/${ConfigName}},
        ${workspace_loc:/ArduinoTools/${ConfigName}} if you need them

if you need to compile for several target hardwares :
*  Project -> edit properties -> AVR :
  *    select "Enable individual settings"
  *    AVR -> Target hardware :
    *      Manage configuration : create one per device, named Uno, Mega ...
        for the first one, rename the "Release" and for next ones, duplicate it
        => all other options are kept
      Select each one and specify target hardware for each (see before)
