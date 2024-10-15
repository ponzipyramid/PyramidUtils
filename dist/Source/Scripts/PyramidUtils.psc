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

GlobalVariable function GetGlobal(string asEditorID) global native

; geography
ObjectReference function GetQuestMarker(Quest akQuest) global native

float function GetAbsDistRefRef(ObjectReference akRef1, ObjectReference akRef2) global native
float function GetAbsDistRefPos(ObjectReference akRef1, float afX, float afY, float aZ) global native
float function GetAbsDistPosPos(float afX1, float afY1, float aZ1, float afX2, float afY2, float afZ2) global native

float function GetAbsPosX(ObjectReference akRef) global native
float function GetAbsPosY(ObjectReference akRef) global native
float function GetAbsPosZ(ObjectReference akRef) global native

; custom console proxy functions
string function ConsoleGetAbsPos(Form akRef) global
    Debug.Trace("ConsoleGetAbsPos - " + akRef)
    
    ObjectReference ref = akRef as ObjectReference

    if !ref
        return "not a valid ref"
    endIf

    float x = GetAbsPosX(ref)
    float y = GetAbsPosY(ref)
    float z = GetAbsPosZ(ref)

    return "Position: (" + x + ", " + y + ", " + z + ")"
endFunction

string function ConsoleGetPlayerAbsDist(Form akRef) global
    ObjectReference ref = akRef as ObjectReference

    if !ref
        return "not a valid ref"
    endIf

    return "Distance: " + GetAbsDistRefRef(Game.GetPlayer(), ref)
endFunction