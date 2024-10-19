Scriptname PyramidUtils Hidden 

Float Function GetVersion() global
    return 0.002010
EndFunction

; Actor
Function SetActorCalmed(Actor akActor, bool abCalmed) global native
Function SetActorFrozen(Actor akTarget, bool abFrozen) global native
Actor[] Function GetDetectedBy(Actor akActor) global native
Keyword[] Function WornHasKeywords(Actor akActor, Keyword[] akKwds) global native
Keyword[] Function WornHasKeywordStrings(Actor akActor, string[] akKwds) global native
Function Dismount(Actor akTarget) global native

; Inventory Processing
Form[] Function GetItemsByKeyword(ObjectReference akContainer, Keyword[] akKeywords, bool abMatchAll = false) global native
Form[] Function FilterFormsByKeyword(Form[] akForms, Keyword[] akKeywords, bool abMatchAll = false, bool abInvert = false) global native
Form[] Function FilterFormsByGoldValue(Form[] akForms, int aiValue, bool abGreaterThan = true, bool abEqual = true) global native
Form[] Function FilterByEnchanted(ObjectReference akContainer, Form[] akForms, bool abEnchanted = true) global native
Form[] Function FilterByEquippedSlot(Form[] akForms, int[] aiSlots, bool abAll = false) global native
Int Function RemoveForms(ObjectReference akFromCont, Form[] akForms, ObjectReference akToCont = none) global native

; Form Processing
bool Function FormHasKeyword(Form akItem, Keyword[] akKwds, bool abAll = false) global native
bool Function FormHasKeywordStrings(Form akItem, string[] akKwds, bool abAll = false) global native

; Player
Actor Function GetPlayerSpeechTarget() global native

; String Processing
String Function ReplaceAt(String asStr, int aiIndex, String asReplace) global native

; Input
String Function GetButtonForDXScanCode(int aiCode) global native
Function RegisterForAllAlphaNumericKeys(Form akForm) global
    akForm.RegisterForKey(57)

    int i = 2
    while i < 12
        akForm.RegisterForKey(i)
        i += 1
    endWhile

    i = 16
    while i < 26
        akForm.RegisterForKey(i)
        i += 1
    endWhile

    i = 30
    while i < 39
        akForm.RegisterForKey(i)
        i += 1
    endWhile

    i = 44
    while i < 51
        akForm.RegisterForKey(i)
        i += 1
    endWhile
EndFunction

Form[] function GetInventoryNamedObjects(ObjectReference akContainer, string[] asNames) global native

; unlike ObjecReference.GetItemHealthPercent, this will work on items in a container (range: 0.0-1.6)
float function GetTemperFactor(ObjectReference akContainer, Form akItem) global native

; geography
ObjectReference function GetQuestMarker(Quest akQuest) global native

; if cell is exterior gets worldspace like normal, if interior looks for external doors and their worldspace
WorldSpace[] function GetExteriorWorldSpaces(Cell akCell) global native
Location[] function GetExteriorLocations(Cell akCell) global native

; unlike GetDistance this works even when one or both refs are in an interior or another cell
float function GetTravelDistance(ObjectReference akRef1, ObjectReference akRef2) global native

; uses worldspace offsets to get absolute position on external refs
float function GetAbsPosX(ObjectReference akRef) global native
float function GetAbsPosY(ObjectReference akRef) global native
float function GetAbsPosZ(ObjectReference akRef) global native

; misc 
GlobalVariable function GetGlobal(string asEditorID) global native

; custom console proxy functions - ignore these
string function ConsoleGetAbsPos(Form akRef) global
    Debug.Trace("ConsoleGetAbsPos - " + akRef)
    
    ObjectReference ref = akRef as ObjectReference

    if !ref
        return "not a valid ref"
    endIf

    float x = GetAbsPosX(ref)
    float y = GetAbsPosY(ref)
    float z = GetAbsPosZ(ref)

    Location loc = ref.GetCurrentLocation()

    string msg = ""
    while loc
        if loc
            msg += loc.GetName() + "\n"
        endIf

        Location parLoc = PO3_SKSEFunctions.GetParentLocation(loc)
        if parLoc != loc
            loc = parLoc
        else
            loc = none
        endIf
    endWhile

    msg += "Position: (" + x + ", " + y + ", " + z + ")"

    return msg
endFunction

string function ConsoleGetPlayerAbsDist(Form akRef) global
    ObjectReference ref = akRef as ObjectReference

    if !ref
        return "not a valid ref"
    endIf

    return GetTravelDistance(Game.GetPlayer(), ref)
endFunction