Scriptname PyramidUtils Hidden 

Float Function GetVersion() global
    return 0.002002
EndFunction

; Actor
Function SetActorCalmed(Actor akActor, bool abCalmed) global native
Function SetActorFrozen(Actor akTarget, bool abFrozen) global native
Actor[] Function GetDetectedBy(Actor akActor) global native

; Inventory Processing
Form[] Function GetItemsByKeyword(ObjectReference akContainer, Keyword[] akKeywords, bool abMatchAll = false) global native
Form[] Function FilterFormsByKeyword(Form[] akForms, Keyword[] akKeywords, bool abMatchAll = false, bool abInvert = false) global native
Form[] Function FilterFormsByGoldValue(Form[] akForms, int aiValue, bool abGreaterThan = true, bool abEqual = true) global native
Form[] Function FilterByEnchanted(ObjectReference akContainer, Form[] akForms, bool abEnchanted = true) global native
Int Function RemoveForms(ObjectReference akFromCont, Form[] akForms, ObjectReference akToCont = none) global native

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

;API function for applying expression preset on actor
;        =Arguments=
;akActor            = actor to process
;apExpression       = raw expression construct.
;aiStrength         = strength which will be used to modify expression. Range 1-100
;abOpenMouth        = if true, will edit phonems to open mouth little
;aiPriority         = expression priority. This needs to be bigger that already applied expression, otherwise expression will not applied
;        =Return value=
;Return true in case that expression was successfully applied
; Apply expression from aaExpression to actor akActor
bool Function ApplyExpressionPreset(Actor akActor, float[] aaExpression, bool abOpenMouth, int exprPower, float exprStrModifier, float modStrModifier, float phStrModifier, float afSpeed, int aiDelay)  global native 
bool function SetPhonemeModifierSmooth(Actor akActor, int aiMode, int aId1, int aiId2, int aiValue, float afSpeed, int aiDelay) global native
bool function SetExpressionSmooth(Actor akActor, int aiMood, int aiStrength, int aiCurrentStrength, float afModifier, float afSpeed, int aiDelay) global native
bool function ResetMFGSmooth(Actor akActor, float afSpeed, int aiDelay) global native

; get phoneme/modifier/expression
int function GetPhonemeValue(Actor act, int id) global native
int function GetModifierValue(Actor act, int id) global native

; return expression value which is enabled. (enabled only one at a time.)
int function GetExpressionValue(Actor act) global native
; return expression ID which is enabled.
int function GetExpressionId(Actor act) global native

