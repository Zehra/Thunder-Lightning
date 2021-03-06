#ifndef FLIGHT_GUNSIGHT_H
#define FLIGHT_GUNSIGHT_H

#include <modules/ui/Label.h>
#include "gunsight.h"

struct IFontMan;
struct IGame;
class FlightInfo;
class JRenderer;

struct CrosshairModule : public UI::Component {
	CrosshairModule();
    void draw(UI::Panel &);
};

struct HUDFrameModule : public UI::Component {
	HUDFrameModule();
    virtual void draw(UI::Panel &);
    virtual void onLayout(UI::Panel &);
};

class SpeedModule : public UI::Label {
	FlightInfo& flight_info;
public:
	SpeedModule(Ptr<IGame>, FlightInfo&);
    virtual std::string getText();
};

class HeightModule : public UI::Label {
	FlightInfo& flight_info;
public:
	HeightModule(Ptr<IGame>, FlightInfo&);
    virtual std::string getText();
};

class HeightGraphModule : public UI::Component {
    FlightInfo& flight_info;
    Ptr<IFontMan> fontman;
public:
    HeightGraphModule(Ptr<IGame>, FlightInfo&);
    void draw(UI::Panel &);
    virtual void onLayout(UI::Panel &);
};

class SpeedGraphModule : public UI::Component {
    FlightInfo& flight_info;
    Ptr<IFontMan> fontman;
public:
    SpeedGraphModule(Ptr<IGame>, FlightInfo&);
    void draw(UI::Panel &);
    virtual void onLayout(UI::Panel &);
};

class HorizonIndicator : public UI::Component {
	FlightInfo& flight_info;
	Ptr<IFontMan> fontman;
public:
	HorizonIndicator(Ptr<IGame>, FlightInfo &, float w, float h);
	void draw(UI::Panel &);
	void drawIndicator(JRenderer *, int degrees, Vector center, Vector right);
};

class GearHookIndicator : public UI::Component {
	Ptr<DataNode> controls;
	Ptr<IFontMan> fontman;
public:
	GearHookIndicator(Ptr<IGame>, Ptr<DataNode> controls);
	void draw(UI::Panel &);
};

class ControlIndicator : public UI::Component {
    Ptr<DataNode> controls;
public:
    struct Axis {
        std::string name;
        bool enabled;
        bool invert;
        bool nonnegative;
        
        inline Axis() : enabled(false), invert(false), nonnegative(false) { }
        inline Axis(const char *name, bool invert, bool nonneg)
        : name(name), enabled(true), invert(invert), nonnegative(nonneg)
        { }
    } axis_x, axis_y;
    
    inline ControlIndicator(const char *name, Ptr<DataNode> controls)
    : UI::Component(name, 100, 100)
    ,controls(controls)
    { }
    
    void draw(UI::Panel&);
};

#endif
