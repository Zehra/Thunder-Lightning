#include <algorithm>
#include <modules/math/Feedback.h>
#include "autopilot.h"


#define MAX_AILERON   0.3f
#define MIN_AILERON  -0.3f
#define EMERGENCY_PITCH  1.2f
#define MAX_PITCH     0.9
#define MIN_PITCH    -0.7f
#define MAX_ELEVATOR  0.9f
#define MIN_ELEVATOR -0.9f
#define MAX_ROLL      1.2f
#define MIN_ROLL     -1.2f
#define MAX_RUDDER    0.3f
#define MIN_RUDDER   -0.3f
#define MIN_ACCEL    (9.81f * -2.0f)
#define MAX_ACCEL    (9.81f *  3.0f)

#define PI 3.14159265358979323846

void AutoPilot::fly( const FlightInfo & fi, FlightControls & ctrls ) {
    if (isEnabled(AP_SPEED)) handleSpeed(fi, ctrls);
    if (isEnabled(AP_COURSE)) handleCourse(fi, ctrls);
    if (isEnabled(AP_ROLL)) handleRoll(fi, ctrls);
    if (isEnabled(AP_HEIGHT)) handleHeight(fi, ctrls);
    if (isEnabled(AP_ALTITUDE)) handleAltitude(fi, ctrls);
    if (isEnabled(AP_PITCH)) handlePitch(fi, ctrls);
    if (isEnabled(AP_ACCEL)) handleAccel(fi, ctrls);
}

#define NUM_MODES 7
bool AutoPilot::setMode( int m ) {
    mode = 0;
    for(int i=0; i<NUM_MODES; i++) {
        if ( m & (2<<i) ) {
            int new_mode = implies(i);
            if (mode & new_mode) {
                mode = 0;
                ls_warning("AutoPilot: Illegal mode %d!\n", mode);
                return false;
            }
            mode |= new_mode;
        }
    }
    return true;
}

bool AutoPilot::enable( int module ) {
    if (mode & (2<<module)) {
        ls_warning("AutoPilot: Cannot enable module %d!\n", module);
        return false;
    }
    mode |= 2<<module;
    return true;
}

int AutoPilot::implies( int module ) {
    switch(module) {
        case AP_SPEED: return AP_SPEED_MASK | AP_ACCEL_MASK;
        case AP_ACCEL: return AP_ACCEL_MASK;
        case AP_ROLL: return AP_ROLL_MASK;
        case AP_PITCH: return AP_PITCH_MASK;
        case AP_HEIGHT: return AP_HEIGHT_MASK | AP_PITCH_MASK; 
        case AP_ALTITUDE: return AP_ALTITUDE_MASK | AP_PITCH_MASK;
        case AP_COURSE: return AP_COURSE_MASK | AP_ROLL_MASK;
    }
    return 0;
}


// #define SPEED_TOLERANCE 0.05
// #define LERP(u,v,t,a,b) (u + v * ((t-a)/(b-a)))
// void AutoPilot::handleSpeed( const FlightInfo & fi, FlightControls & ctrls ) {
//     float speed = fi.getCurrentSpeed();
//     float min_band = speed_target * 0.8;
//     float max_band = speed_target * 1.2;
//     float min_tolerance = speed_target - SPEED_TOLERANCE;
//     float max_tolerance = speed_target + SPEED_TOLERANCE;
//     float accel_target;
//     
//     if (speed < min_band) accel_target = MAX_ACCEL;
//     else if (speed > max_band) accel_target = MIN_ACCEL;
//     else if (speed < min_tolerance)
//         accel_target = LERP(MAX_ACCEL, 0, speed, min_band, min_tolerance);
//     else if (speed > max_tolerance)
//         accel_target = LERP(0, MIN_ACCEL, speed, max_tolerance, max_band);
//     else accel_target = 0.0f;
//     
//     //ls_message("handleSpeed: speed=%f speed_target=%f -> accel_target=%f\n",
//     //        speed, speed_target, accel_target);
//     
//     setTargetAccel(accel_target);
// }

void AutoPilot::handleSpeed( const FlightInfo & fi, FlightControls & ctrls ) {
    float x = fi.getCurrentSpeed();
    float dx = fi.getCurrentAccel();
    FeedbackController<float> controller(
            speed_target,
            MAX_ACCEL / 100.0f,
            -MAX_ACCEL / 50.0f);
    float r = controller.control(x, dx);
    setTargetAccel(r);
}

void AutoPilot::handleAccel( const FlightInfo & fi, FlightControls & ctrls ) {
    float accel = fi.getCurrentAccel();
    float accel_error = accel_target - accel;
    float throttle_adj = ctrls.getThrottle() + accel_error*0.001;
    throttle_adj = std::max(0.0f, std::min(1.0f, throttle_adj));
    //ls_message("handleAccel: accel=%f accel_target=%f -> trottle=%f\n",
    //        accel, accel_target, throttle_adj);
    ctrls.setThrottle(throttle_adj);
}

// #define ROLL_TOLERANCE 0.5f
// void AutoPilot::handleRoll( const FlightInfo & fi, FlightControls & ctrls ) {
//     float roll_error = roll_target - fi.getCurrentRoll();
//     while (roll_error >  PI) roll_error-=PI;
//     while (roll_error < -PI) roll_error+=PI;
//     float min_band = -PI/8.0f;
//     float min_tolerance = -ROLL_TOLERANCE;
//     float max_band = PI/8.0f;
//     float max_tolerance = ROLL_TOLERANCE;
//     float aileron_adj;
//     
//     if (roll_error < min_band) aileron_adj = MAX_AILERON;
//     else if (roll_error > max_band) aileron_adj = MIN_AILERON;
//     else if (roll_error < min_tolerance)
//         aileron_adj = LERP(MAX_AILERON, 0.0f, roll_error, min_band, min_tolerance);
//     else if (roll_error > max_tolerance)
//         aileron_adj = LERP(0.0f, MIN_AILERON, roll_error, max_tolerance, max_band);
//     else aileron_adj = 0.0f;
//     
//     //ls_message("handleRoll: roll=%f roll_target=%f roll_error=%f -> aileron=%f\n",
//     //        fi.getCurrentRoll() * 180.0 / PI,
//     //        roll_target * 180.0 / PI,
//     //        roll_error * 180.0 / PI,
//     //        aileron_adj);
//     
//     //ls_warning("current_roll = %f roll_error= %f\n", fi.getCurrentRoll(), roll_error);
//     
//     ctrls.setAileron(-aileron_adj);
//     //if (roll_error > 0) ctrls.setAileronAndRudder(1);
//     //else ctrls.setAileronAndRudder(-1);
// }

void AutoPilot::handleRoll( const FlightInfo & fi, FlightControls & ctrls ) {
    float x = fi.getCurrentRoll();
    float dx = fi.getCurrentRollSpeed();
    FeedbackController<float> controller(
            roll_target,
            MAX_AILERON / (45 * PI / 180),
            MIN_AILERON / (45 * PI / 180));
    float r = controller.control(x, dx);
    if (r > MAX_AILERON) r = MAX_AILERON;
    else if (r < MIN_AILERON) r = MIN_AILERON;
//     cerr << "AutoPilot: roll_target: " << roll_target << endl;
//     cerr << "AutoPilot: current_roll: " << x << endl;
//     cerr << "AutoPilot: current_roll_speed: " << dx << endl;
//     cerr << "AutoPilot: Setting aileron to " << r << endl;
    ctrls.setAileron(r);
}    

// void AutoPilot::handlePitch( const FlightInfo & fi, FlightControls & ctrls ) {
//     //ls_message("pitch: %f\n",fi.getCurrentPitch());
//     float pitch_error = pitch_target - fi.getCurrentPitch();
//     while (pitch_error >  PI) pitch_error-=PI;
//     while (pitch_error < -PI) pitch_error+=PI;
//     float elevator_adj = std::max(-0.7f, std::min( 0.7f, -pitch_error));
//     ctrls.setElevator(elevator_adj);
// }

void AutoPilot::handlePitch( const FlightInfo & fi, FlightControls & ctrls ) {
    float x = fi.getCurrentPitch();
    float dx = fi.getCurrentPitchSpeed();
    FeedbackController<float> controller(
            pitch_target,
            MIN_ELEVATOR / (45 * PI / 180),
            MAX_ELEVATOR / (45 * PI / 180));
    float r = controller.control(x, dx);
    float elevator_adj = std::max(-0.7f, std::min( 0.7f, r));
    ctrls.setElevator(elevator_adj);
}

// void AutoPilot::handleHeight( const FlightInfo & fi, FlightControls & ctrls ) {
//     float ground_altitude = fi.getCurrentAltitude() - fi.getCurrentHeight();
//     setTargetAltitude(height_target + ground_altitude);
// }

void AutoPilot::handleHeight( const FlightInfo & fi, FlightControls & ctrls ) {
    float x = fi.getCurrentHeight();
    float dx = fi.getCurrentDHeight();
    FeedbackController<float> controller(
            height_target,
            MAX_PITCH / 100.0f,
            MIN_PITCH / 100.0f);
    float r = controller.control(x, dx);
    if (r > MAX_PITCH) r = MAX_PITCH;
    else if (r < MIN_PITCH) r = MIN_PITCH;
    setTargetPitch(r);
}


// void AutoPilot::handleAltitude( const FlightInfo & fi, FlightControls & ctrls ) {
//     float altitude_error = altitude_target - fi.getCurrentAltitude();
//     float e = altitude_error;
//     altitude_error *= std::abs(altitude_error*altitude_error);
//     float dec_height = std::min(0.0f, std::min(fi.getCurrentHeight(), 100.0f));
//     altitude_error /= dec_height*dec_height*dec_height*MAX_PITCH;
//     float pitch_adj = std::max(MIN_PITCH, std::min(MAX_PITCH, altitude_error));
//     ls_message("Altitude error: %4.2f (target: %4.2f) "
//             "==> pitch target: %3.2f\n",
//             e, altitude_target, pitch_adj);
//     setTargetPitch(pitch_adj);
// }

// void AutoPilot::handleAltitude( const FlightInfo & fi, FlightControls & ctrls ) {
//     float altitude_error = altitude_target - fi.getCurrentAltitude();
//     float e = altitude_error;
//     altitude_error *= abs(altitude_error*altitude_error);
//     float dec_height = max(0.0f, min(fi.getCurrentHeight(), 100.0f));
//     altitude_error /= dec_height*dec_height*dec_height*MAX_PITCH;
//     float pitch_adj = max(MIN_PITCH, min(MAX_PITCH, altitude_error));
//     ls_message("Altitude error: %4.2f (target: %4.2f) "
//             "==> pitch target: %3.2f\n",
//             e, altitude_target, pitch_adj);
//     setTargetPitch(pitch_adj);
// }

void AutoPilot::handleAltitude( const FlightInfo & fi, FlightControls & ctrls ) {
    float band_max = altitude_target + 200.0f;
    float ground_altitude = fi.getCurrentAltitude() - fi.getCurrentHeight();
    float band_min = std::max(altitude_target - 200.0f, ground_altitude + 100.0f);

    float altitude = fi.getCurrentAltitude();
    float pitch;

    if (fi.collisionWarning()) pitch = EMERGENCY_PITCH;
    else if (altitude > band_max) pitch = MIN_PITCH;
    else if (altitude < band_min) pitch = MAX_PITCH;
    else if (altitude < altitude_target) {
        float target = std::max(altitude_target - 10.0f, altitude);
        pitch =  MAX_PITCH;
        pitch *= (altitude - band_min) / (target - band_min);
    } else {
        float target = std::min(altitude_target + 10.0f, altitude);
        pitch = MIN_PITCH;
        pitch *= (altitude - target) / (band_max - target);
    }

    setTargetPitch(pitch);
}

void AutoPilot::handleCourse( const FlightInfo & fi, FlightControls & ctrls ) {
    float course_error = course_target - fi.getCurrentCourse();
    float d_course = fi.getCurrentCourseSpeed();
    while (course_error >  PI) course_error -= 2.0*PI;
    while (course_error < -PI) course_error += 2.0*PI;
    FeedbackController<float> controller(
            0.0f,
            -16.0 / PI,
            -4.0f / PI);
    float r = controller.control(course_error, d_course);
    r = std::min(1.0f, std::max(-1.0f, r));
    setTargetRoll(MAX_ROLL * r);
    ctrls.setRudder(MAX_RUDDER * r);
}
