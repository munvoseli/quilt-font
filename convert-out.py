#!/bin/python3

import fontforge
quilt = fontforge.open("out.sfd")
quilt.generate("out.woff2")
