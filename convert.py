#!/bin/python3

import fontforge
quilt = fontforge.open("Quilt.sfd")
quilt.generate("Quilt.woff2")
