Sequence say := method(
  Game viewSubject passMessage("infoMessage", Object clone do(text := self))
  Nil
)

Game on("toggle-introduction",
    if (intro running, intro interrupt, intro init; intro start)
)

choose := method(
    " In choose" println
    n := call argCount
    " Random draw" println
    r := n * Random value
    (" r = " .. r .. "Nil test") println
    if (n == 0, " Nil!" println; return Nil)
    for(i,1,n,
        "  In for, i:" ..(i) println
        if (r <= i,
            "  Evaluating " .. (call argAt(i) code) println
            x := call evalArgAt(i)
            "  returning x" println
            ("   (x == " .. x asString .. ")") println
            return x
        )
    )
)

intro := coro(dummy,
    ex := try (
        my_watchdog := watchdog clone
        my_watchdog start(me, self)
        
        "Welcome to Thunder&Lightning!" say
        sleep(8)
        "This is Commander Wilson speaking, your flight instructor." say
        sleep(8)
        "We will go through some basics for your first flight." say
        sleep(10)
        "You can end this introduction at any time by pressing the (I) key." say
        sleep(5)
        "Start it again by pressing the (I) key a second time." say
        sleep(10)
        "Welcome aboard the Lightning, a small fighter." say
        sleep(8) 
        "The Lightning is controlled using the mouse and the keyboard." say
        sleep(2)
        "A joystick can be used optionally." say
        sleep(8)
        "Mouse movements control basic flight. You can also use the cursor keys." say
        sleep(8)
        "The throttle can be set from 0% to 100% using keys 1,2,3,...,8,9,0." say
        sleep(8)
        "First of all, get us into a safer altitude, please." say
        sleep(5)
        "4000 meters should be enough. " say
        sleep(5)
        "Oh, the altitude is indicated on the right side of the HUD." say
        while (Object,
            8 repeatTimes(
                sleep(1)
                if (me getLocation at(1,0) >= 4000,
                    break
                )
            )
            if (me getLocation at(1,0) >= 4000,
                break
            )
            
            "Entering choose" println
            choose(
                "I am waiting for you to climb up to 4000, kid!",
                "I know you can do it!",
                "Just pull the stick and climb, it's easy!",
                "Make sure the nose is over the horizon!",
                "Are you afraid of heights, kid?"
            ) say
            "Exiting choose" println
        )
        "Ok, now we're high enough." say
        sleep(8)
        "Good, you alrady know where the altitude and height indicator is." say
        sleep(8)
        "The small numbers arranged like ladder steps are steps of 50m of altitude." say
        sleep(8)
        "The big number to the left of them displays your height over ground." say
        sleep(8)
        ("Right now, we are " .. (me getLocation at(1,0) floor) .. " meters above ground.") say
        sleep(8)
        "In a similiar setup, you find your airspeed indicator on the left side."
        sleep(8)
        ("Our current speed is " .. ((me getMovementVector length * 3.6) floor) .. "km/h.") say
        sleep(8)
        "On the lower edge of the screen, you can see which weapon is selected" say
        "as your primary and secondary weapon." say
        sleep(8)
        "Change the primary weapon on the left with the Backspace key and" say
        "and fire it with the left mouse button or Ctrl." say
        sleep(8)
        "Switch the secondary weapon on the right with the Enter key and" say
        "fire it with the right mouse button or Space." say
        sleep(8)
        "Your primary weapon is a 4-barrel Vulcan cannon." say
        "Deadly in the hands of a master." say
        sleep(8)
        "Your secondary weapons are Sidewinder and Hydra missiles." say
        sleep(8)
        "The sidewinder is a heat-seeking missile that will find its way" say
        "into your enemy." say
        sleep(8)
        "The Hydra is an unguided but powerful missile," say
        "well-suited against ground targets." say
        sleep(8)
        "To use the Sidewinders, you must lock on a target." say
        sleep(8)
        "Use the T key to cycle through all targets. Use R to reverse-cycle." say
        sleep(8)
        "With G you select the target closest to the center of your gunsight." say
        sleep(8)
        "H and F cycle through hostile and friendly targets, respectively." say
        sleep(8)
        "Finally, Z (or Y) selects the nearest target." say
        sleep(8)
        "Are you ready for a challenge? Let's try your combat skills!" say
        sleep(5)
        
        him := addEnemy
        sleep(3)
        "There's an enemy fighter directly in front of you. Get him!" say
        loop(
            8 repeatTimes(
                sleep(1)
                if (him isAlive not,
                    break
                )
            )
            if (him isAlive not,
                break
            )
            
            r := 3 * Random value
            if (      r < 1,
                "Remember you can select hostile targets with the H key!" say
            ) elseif( r < 2,
                "Use your Sidewinder missiles!" say
            ) else(
                "Get him before he does!" say
            )
        )
        
        "That was it, you got him! Congratulations!" say
        sleep(8)
        "This is the end of the introduction." say
        sleep(4)
        "Now have some fun flying, evading and shooting!" say
        sleep(4)
        "And remember you can restart this introduction with the (I) key!" say
        sleep(8)
        "Thunder&Lightning was programmed by Jonas Eschenburg" say
        sleep(4)
        "Special thanks to Sam Lantinga, Steve Dekorte and others." say
        
        my_watchdog interrupt
    )
    ex catch(InterruptedException,
        interruptReq = Nil
        if(me isAlive,
            "Introduction was aborted. Restart with (I) key." say
        ,
            "I think he's crashed, Jim!" say
            sleep(8)
            "Yeah, seems this novice pilot wasn't so smart after all!" say
            sleep(5)
            "Get me a new one, will you?" say
        )
    ) catch(Exception,
        ex showStack
    )
)

setupScene := method(
    Lobby us := Faction clone do (
        setName("Green")
        setDefaultAttitude(HOSTILE)
        setAttitudeTowards(thisContext, FRIENDLY)
    )
    Lobby me := Drone clone
    Game addActor(me)
    me setLocation(vector(11341,1518,-1008))
    me setMovementVector(vector(-98.71, -14.53, -57.98))
    me setOrientation(matrix(
        -0.496, -0.181, -0.849
        -0.101,  0.983, -0.151
        0.862,  0.011, -0.506))
    me setFaction(us)
    
    Game setControlledActor(me)
    Game setView(me, 0)
)

addEnemy := method(
    them := Faction clone do (
        setName("Red")
        setDefaultAttitude(HOSTILE)
        setAttitudeTowards(thisContext, FRIENDLY)
    )
    Lobby him := Drone clone
    Game addActor(him)
    him setLocation(me getLocation + (1000*(me getFrontVector)))
    him setMovementVector(me getMovementVector)
    him setOrientation( me getOrientation )
    him setFaction(them)
    him setControlMode(Actor AUTOMATIC)
    return him
)

watchdog := coro(actor, target_coro,
    while (actor isAlive,
        pass(0.5)
    )
    target_coro interrupt
)

setupScene
intro start
