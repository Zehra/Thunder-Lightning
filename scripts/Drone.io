Stats := Object clone do (
    lastDamagers := Map clone
    kills := List clone
    
    registerDamage := method(damagingActor, damagedActor, damageValue,
        lastDamagers atPut(damagedActor uniqueId asString, damagingActor getFaction getName)
    )
    
    registerKill := method( victim,
        lastDamager := lastDamagers at( victim uniqueId asString)
        killentry := Object clone
        if (lastDamager,
            killentry culprit := lastDamager
            killentry victim := ((victim getFaction) getName)
        ,
            killentry culprit := "<suicide>"
            killentry victim := ((victim getFaction) getName)
        )
        kills append(killentry)
    )
    
    killsBy := method( faction,
        kills select(i,v, v culprit == faction) size
    )

    killsOf := method( faction,
        kills select(i,v, v victim == faction) size
    )

    killsOfBy := method( culprit, victim,
        kills select(i,v, v culprit == culprit and v victim == victim) size
    )
    
    scoreOf := method( faction, killsBy(faction) - killsOf(faction))
    
    allFactions := method(
        factions := Map clone
        kills foreach(i,v, factions atPut(v culprit, Nil); factions atPut(v victim, Nil))
        factions keys
    )
    
    print := method(
        factions := allFactions
        factions sortBy (method(a,b, scoreOf(a) > scoreOf(b)))
        writeln("-----------------------------------")
        writeln("         Faction score kills deaths")
        factions foreach(i, faction,
            writeln(faction, " ", scoreOf(faction), " ", killsBy(faction), " ", killsOf(faction))
        )
        
        writeln("-----------------------------------")
    )
)

stats := Stats
Drone do(
    inspect
    /*
    delegate(asPositionProvider, asMovementProvider) to (asActor)
    delegate(asPositionReceiver, asMovementReceiver) to (asActor)
    delegate(getFaction, getNumViews) to (asActor)
    delegate(getFrontVector, getRightVector, getUpVector) to (asPositionProvider)
    delegate(getLocation, getOrientation) to(asPositionProvider)
    delegate(setLocation, setOrientation) to(asPositionReceiver)
    delegate(getMovementVector) to(asMovementProvider)
    delegate(setMovementVector) to(asMovementReceiver)
    delegate(message, on) to(asActor or Actor)
    */

    onKill := method(
        stats registerKill(self)
    )
    
    onDamage := method(damage, domain, projectile, source,
        if (source isNil, return)
        stats registerDamage(source, self, damage)
    )
)
