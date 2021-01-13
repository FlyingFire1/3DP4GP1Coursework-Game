#pragma once
#include <string>

#include "MenuMgr.h"


/*
Creates a small window frame with two buttons in it
The code to configure the elements is long and cumbersome so 
wrapping in a reusable function like this makes sense.
mgr - the one and only menumgr
root - a root node that this two button setup can attach to, so if you want this two button group 
on ten different menu screens then just call it ten times and attach to ten different roots
b1,b2 - what the buttons should have written on them
b1Name,b2Name - this is the internal logical name for the buttons, so you cna hook up event handlers to them
*/
void BuildTwoButtonUI(MenuMgr& mgr, MenuNode& root, float x, float y, const std::string& b1, const std::string& b1Name, const std::string& b2, const std::string& b2Name);
