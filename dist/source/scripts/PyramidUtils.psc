Scriptname PyramidUtils Hidden 

Function CalmActor(Actor akActor) global native

Function ClearActor(Actor akActor) global native

Actor[] Function GetActorsDetectedBy(Actor akActor, Actor[] akSeenBy) global native

Form[] Function GetItemsByKeyword(ObjectReference akContainer, Keyword[] akKeywords, bool abMatchAll = false) global native

Function RemoveItemsByKeyword(ObjectReference akFromCont, ObjectReference akToCont, Keyword[] akKeywords, bool abMatchAll = false) global native