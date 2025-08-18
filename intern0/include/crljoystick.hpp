//      crljoystick.hpp
//      
//      Copyright 2013 IGARASHI <igarashi@siesta>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


//	=========================================================================
//
//	windowsプログラム使用の場合は，#include "crljoystick.hpp" より上で
//	#define WIN32 を指定（Linuxの場合は UNIX）
//	※ crlmath3.hpp と共通
//
//	 crlJoystick js;
//	 double u;
//	 js.init();
//	 u = js.get_axis(0); // 0: x軸, 1: y軸, 2: 回転軸，3: スライダ
//
//
//	=========================================================================

#ifndef __CRL_JOYSTICK_HPP_130227__
#define __CRL_JOYSTICK_HPP_130227__

#include <iostream>
#include <stdio.h>

#ifdef UNIX
#ifndef APPLE // LINUX_NATIVE
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#endif
#endif

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <float.h>
#pragma comment (lib, "winmm.lib")
#endif

#ifdef APPLE
#include <IOKit/hid/IOHIDLib.h>
#endif

#define JOY_DEV "/dev/input/js0"
#define CRL_JS_AXIS_NUM 4
#define CRL_JS_BUTTON_NUM 8
#define JS2_EPS 0.01

class crlJoystick {
private:
    //double m_axis[CRL_JS_AXIS_NUM];    //	ジョイスティックの軸情報 [-1:1]
    //bool m_button[CRL_JS_BUTTON_NUM];    //	ジョイスティックのボタン情報 (true: 押下)

#ifdef UNIX
#ifndef APPLE
    int m_joy_fd;	//	joystick filedescriptor
#endif
#endif

#ifdef WIN32
    JOYINFOEX joyinfo;
    UINT wNumDevs, wDeviceID;
#endif

#ifdef APPLE
    IOHIDManagerRef hid_manager;
    CFMutableArrayRef matcher;
    CFIndex number_of_devices;
    CFSetRef device_set;
    IOHIDDeviceRef *device_array;
    IOHIDDeviceRef *current;
    CFStringRef manufacturer, product_name;
    char string_buffer[1024];
#endif

    bool active_flg;

public:
    crlJoystick() {
        active_flg = false;
    };

    ~crlJoystick() {};

    //	JOYSTICK の初期化(成功すれば init_flg = true)
    bool init(const char *joydev_name = JOY_DEV) {

#ifdef UNIX
#ifndef APPLE
        if( ( m_joy_fd = open( joydev_name , O_RDONLY)) == -1 ) {
            std::cerr << "#error: couldn't open joystick  [LINUX] @crlJoystick::init() " << std::endl;
            return false;
        }
        char name_of_joystick[80];
        int ax_num, bt_num;
        ax_num = CRL_JS_AXIS_NUM;
        bt_num = CRL_JS_BUTTON_NUM;
        ioctl( m_joy_fd, JSIOCGAXES, & ax_num);
        ioctl( m_joy_fd, JSIOCGBUTTONS, & bt_num);
        ioctl( m_joy_fd, JSIOCGNAME(80), &name_of_joystick );
        fcntl( m_joy_fd, F_SETFL, O_NONBLOCK );
#endif
#endif

#ifdef WIN32

        BOOL bDev1Attached, bDev2Attached;

        if((wNumDevs = joyGetNumDevs()) == 0) {
            // ドライバーがない
            std::cerr << "#error: driver is not found! @crlJoystick::init()" << std::endl;
            active_flg = false;
            return false;
        }

        bDev1Attached = joyGetPosEx(JOYSTICKID1, &joyinfo) != JOYERR_UNPLUGGED;
        bDev2Attached = wNumDevs == 2 && joyGetPosEx(JOYSTICKID2, &joyinfo) != JOYERR_UNPLUGGED;

        if(bDev1Attached || bDev2Attached) {  // どのjoysticIDを使うか決定する
            wDeviceID = bDev1Attached ? JOYSTICKID1 : JOYSTICKID2;
            active_flg = true;
        } else {
            std::cerr << "#error: joystick connection error! [WIN] @crlJoystick::init()" << std::endl;
            active_flg = false;
            return false;
        }
        JOYCAPS jc;
        MMRESULT mmres;
        if((mmres = joyGetDevCaps(wDeviceID, (LPJOYCAPS) &jc, sizeof(jc))) != JOYERR_NOERROR) {
            std::cout << "#error: joyGetDevCaps() returns error! @crlJoystick::init()" << std::endl;
            active_flg = false;
            return false;
        };
        std::cout << "#debug; JOYCAPS: buttons: "<<jc.wMaxButtons << ", ";
        std::cout << "(xmin, xmax): (" << jc.wXmin << ", " << jc.wXmax << ") ";
        std::cout << "(ymin, ymax): (" << jc.wYmin << ", " << jc.wYmax << ") " << std::endl;

        std::cout << "#debug: wNumDevs: " << wNumDevs << "@crlJoystick::init()"<< std::endl;
        std::cout << "#debug: wDeviceID: " << wDeviceID << "@crlJoystick::init()"<< std::endl;
#endif
#ifdef APPLE
        hid_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
        matcher = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
        if (!matcher) {
            std::cerr << "#error: matcher initialize failed! @crlJoystick::init()" << std::endl;
            return false;
        }

        append_matching_dictionary(matcher, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
        append_matching_dictionary(matcher, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);

        IOHIDManagerSetDeviceMatchingMultiple(hid_manager, matcher);

        //DeviceOpen
        IOHIDManagerOpen(hid_manager, kIOHIDOptionsTypeNone);
        CFRelease(matcher);

        device_set = IOHIDManagerCopyDevices(hid_manager);
        number_of_devices = device_set == nullptr ? 0 : CFSetGetCount(device_set);

        if (number_of_devices == 0) {
            std::cerr << "#error[APPLE]: No Joystick detected. @crlJoystick::init() " << std::endl;
            return false;
        } else {
            std::cout << number_of_devices << " HID devices found" << std::endl;
            //Get the list into a C++ array
            device_array = new IOHIDDeviceRef[number_of_devices];
            CFSetGetValues(device_set, (const void **) device_array);
            for (int i = 0; i < number_of_devices; i++) {
                current = &device_array[i];
                std::cout << "No. " << i << ": ";
                show_device_name(current);
//        show_elements(current);
            }
        }

#endif
        active_flg = true;
        std::cout << "#debug: jrlJoystick::init() succeeded!" << std::endl;
        return active_flg;
    };


    //	axis button にそれぞれ現在の値を格納
    bool get(double *axis, bool *button) const {
        if (!active_flg) {
            std::cerr << "#error: joystick is not active! @crlJoystick::get()" << std::endl;
            return false;
        }
        _get_js_data(axis, button);
        return true;
    };

    //	id番目のAXISを返す
    bool get_axis(const int dim, std::vector<double> &axisv) const{
        axisv.resize(dim);
        if (!active_flg) {
            std::cerr << "#error: joystick is not active! @crlJoystick::get_x()";
            std::cerr << std::endl;
            return false;
        }
        double axis[CRL_JS_AXIS_NUM];
        bool button[CRL_JS_BUTTON_NUM];
        _get_js_data(axis, button);
        for(int i=0; i<dim; i++) {
            axisv[i] = axis[i];
        }
        return true;
    }

        //	id番目のAXISを返す
    double get_axis(int id) const {
        if (!active_flg) {
            std::cerr << "#error: joystick is not active! @crlJoystick::get_x()";
            std::cerr << std::endl;
            return 0.0;
        }
        if (id < 0 || id > CRL_JS_AXIS_NUM) {
            std::cerr << "#error: id is incorrect! [id: " << id;
            std::cerr << "] @crlJoystick::get_axis()" << std::endl;
            return 0.0;
        }
        double axis[CRL_JS_AXIS_NUM];
        bool button[CRL_JS_BUTTON_NUM];
        _get_js_data(axis, button);
        //std::cout << "#debug: m_axis: " << m_axis[0] << ", " << m_axis[1] << std::endl;
        return axis[id];
    }

    //	active_flg を返す
    bool check_js(void) const {
        return active_flg;
    }

    //	id番目のBUTTONを返す
    bool get_button(int id)  {
        if (!active_flg) {
            std::cerr << "#error: joystick is not active! @crlJoystick::get_x()" << std::endl;
            return false;
        }
        if (id < 0 || id > CRL_JS_BUTTON_NUM) {
            std::cerr << "#error: id is incorrect! [id: " << id << "] @crlJoystick::get_button()" << std::endl;
            return false;
        }
        double axis[CRL_JS_AXIS_NUM];
        bool button[CRL_JS_BUTTON_NUM];
        _get_js_data(axis, button);
        return button[id];
    };

    //	Joystickがアクティブであればtrueを返す
    inline bool is_active() { return active_flg; };

private:
#ifdef APPLE

    void append_matching_dictionary(CFMutableArrayRef _matcher, uint32_t page, uint32_t use) const {
        CFMutableDictionaryRef result = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                  &kCFTypeDictionaryKeyCallBacks,
                                                                  &kCFTypeDictionaryValueCallBacks);
        if (!result) return;
        CFNumberRef pageNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);
        CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsagePageKey), pageNumber);
        CFRelease(pageNumber);
        CFNumberRef useNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &use);
        CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsageKey), useNumber);
        CFRelease(useNumber);
        CFArrayAppendValue(_matcher, result);
        CFRelease(result);
    }

    void show_device_name(IOHIDDeviceRef *_device)  {
        /*
        Get the manufacturer name (which is a string)
        */
        if ((manufacturer = (CFStringRef) IOHIDDeviceGetProperty(*_device, CFSTR(kIOHIDManufacturerKey))) != NULL) {
            CFStringGetCString(manufacturer, string_buffer, sizeof(string_buffer), kCFStringEncodingUTF8);
            std::cout << string_buffer;
        }
        /*
         Get the product name (which is a string)
         */
        if ((product_name = (CFStringRef) IOHIDDeviceGetProperty(*_device, CFSTR(kIOHIDProductKey))) != NULL) {
            CFStringGetCString(product_name, string_buffer, sizeof(string_buffer), kCFStringEncodingUTF8);
            std::cout << string_buffer;
        }

        std::cout << std::endl;
    }

    void show_elements(IOHIDDeviceRef *_device) const {
        std::cout << "-------Element Info-------" << std::endl;
        CFArrayRef elements = IOHIDDeviceCopyMatchingElements(*_device, NULL, 0);
        if (elements) {
            CFIndex idx, cnt = CFArrayGetCount(elements);

            std::cout << cnt << " Elements found!" << std::endl;

            for (idx = 0; idx < cnt; idx++) {
                auto element = (IOHIDElementRef) CFArrayGetValueAtIndex(elements, idx);
                int type = IOHIDElementGetType(element);
                int page = IOHIDElementGetUsagePage(element);
                int usage = IOHIDElementGetUsage(element);

                switch (type) {
                    case kIOHIDElementTypeInput_Button:
                        std::cout << "kIOHIDElementTypeInput_Button " << type << ", " << page << ", " << usage
                                  << std::endl;
                        break;
                    case kIOHIDElementTypeInput_Axis:
                        std::cout << "kIOHIDElementTypeInput_Axis " << type << ", " << page << ", " << usage
                                  << std::endl;
                        break;
                    case kIOHIDElementTypeInput_Misc:
                        std::cout << "kIOHIDElementTypeInput_Misc " << type << ", " << page << ", " << usage
                                  << std::endl;
                        break;
                }
            }
        }

    }

    int get_value_by_id(IOHIDDeviceRef *_device, int _type, int _id) const{
        IOHIDValueRef value;

        CFArrayRef elements = IOHIDDeviceCopyMatchingElements(*_device, NULL, 0);
        if (elements) {
            CFIndex idx, cnt = CFArrayGetCount(elements);

            for (idx = 0; idx < cnt; idx++) {
                auto element = (IOHIDElementRef) CFArrayGetValueAtIndex(elements, idx);
                int type = IOHIDElementGetType(element);
                int page = IOHIDElementGetUsagePage(element);
                int usage = IOHIDElementGetUsage(element);

                if (type == _type) {
                    if (usage == _id) {
                        IOHIDDeviceGetValue(device_array[0], element, &value);
                        return (int) IOHIDValueGetIntegerValue(value);
                    }
                }
            }
        }
        return -1;
    }
#endif
    bool _get_js_data(double *axis, bool *button) const{

        if(active_flg == false) {
            std::cerr << "#error: joystick is not active! @crlJoystick::_get_js_data()" << std::endl;
            return false;
        }
#ifdef UNIX
        int ret;
        js_event js_ev;

        if((ret = read(m_joy_fd, &js_ev, sizeof(struct js_event))) <= 0) {
            //std::cerr << "#warning: read_err: " << std::endl;
            return false;
        }
        switch (js_ev.type)
        {
            case JS_EVENT_AXIS:
                if(js_ev.number == 0)
                    axis[ 1 ] = -js_ev.value / 32767.0;
                else if(js_ev.number == 1)
                    axis[ 0 ] = -js_ev.value / 32767.0;
                else if (js_ev.number < CRL_JS_AXIS_NUM)
                    axis[ js_ev.number ] = js_ev.value / 32767.0;
                break;
            case JS_EVENT_BUTTON:
                if(js_ev.number < CRL_JS_BUTTON_NUM)
                    button[ js_ev.number ] = js_ev.value;
                break;
        }
#endif
#ifdef WIN32
        for(int i=0; i<CRL_JS_AXIS_NUM; i++) {
            axis[i] = 0.0;
        }
        MMRESULT r=joyGetPosEx((UINT)wDeviceID, (LPJOYINFOEX )&joyinfo);

       if(r==JOYERR_NOERROR) {
           // ボタン判定
            button[0] =(joyinfo.dwButtons & JOY_BUTTON1) ? true : false;
            button[1] =(joyinfo.dwButtons & JOY_BUTTON2) ? true : false;
            button[2] =(joyinfo.dwButtons & JOY_BUTTON3) ? true : false;
            button[3] =(joyinfo.dwButtons & JOY_BUTTON4) ? true : false;
            if(CRL_JS_BUTTON_NUM > 4)
                button[4] =(joyinfo.dwButtons & JOY_BUTTON5) ? true : false;
            if(CRL_JS_BUTTON_NUM > 5)
                button[5] =(joyinfo.dwButtons & JOY_BUTTON6) ? true : false;
            if(CRL_JS_BUTTON_NUM > 6)
                button[6] =(joyinfo.dwButtons & JOY_BUTTON7) ? true : false;
            if(CRL_JS_BUTTON_NUM > 7)
                button[7] =(joyinfo.dwButtons & JOY_BUTTON8) ? true : false;
            if(CRL_JS_BUTTON_NUM > 8)
                button[8] =(joyinfo.dwButtons & JOY_BUTTON9) ? true : false;
            if(CRL_JS_BUTTON_NUM > 9)
                button[9] =(joyinfo.dwButtons & JOY_BUTTON10) ? true : false;

            //	XYZR 判定
            axis[0] = (float)-1.0f+2.0f*(joyinfo.dwXpos/(float)0xffff);
            axis[1] = (float)-1.0f+2.0f*(joyinfo.dwYpos/(float)0xffff);
            axis[2] = (float)-2.0f*(joyinfo.dwRpos/(float)0xffff)+1.0f;
            axis[3] = (float)-2.0f*(joyinfo.dwZpos/(float)0xffff)+1.0f;
            for(int i=0; i<CRL_JS_AXIS_NUM; i++) {
                if(_isnan(axis[i]) != 0)  axis[i] = 0.0;
            }
        }
#endif
#ifdef APPLE
        for(int id=0; id<CRL_JS_AXIS_NUM; id++)
            axis[id] = (double) (get_value_by_id(&device_array[0], kIOHIDElementTypeInput_Misc, id + 48) - 0x7F) / 127.0;
        for(int id=0; id<CRL_JS_BUTTON_NUM; id++)
            button[id] = (bool) get_value_by_id(&device_array[0], kIOHIDElementTypeInput_Button, id + 1);
#endif
        for (int i = 0; i < CRL_JS_AXIS_NUM; i++) {
            if (fabs(axis[i]) < JS2_EPS) axis[i] = 0.;
            if (axis[i] > 1.0) axis[i] = 1.0;
            if (axis[i] < -1.0) axis[i] = -1.0;
        }
        axis[1] = -axis[1];
        return true;
    }
};

#endif
