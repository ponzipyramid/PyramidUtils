Scriptname PyramidUtils Hidden 

String Function GetVersion() global
    return 1
EndFunction

Function CalmActor(Actor akActor) global native

Function ClearActor(Actor akActor) global native

Actor[] Function GetActorsDetectedBy(Actor akActor, Actor[] akWatchers) global native

Form[] Function GetItemsByKeyword(ObjectReference akContainer, Keyword[] akKeywords, bool abMatchAll = false) global native

Form[] Function FilterFormsByKeyword(Form[] akForms, Keyword[] akKeywords, bool abMatchAll = false, bool abInvert = false) global native

Int Function RemoveItemsByKeyword(ObjectReference akFromCont, Keyword[] akKeywords, bool abMatchAll = false, ObjectReference akToCont = none) global native

Actor Function GetPlayerSpeechTarget() global native

float Function GetPlayerDistanceToMarker(ObjectReference akRef) global native