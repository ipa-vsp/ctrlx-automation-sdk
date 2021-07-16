# MIT License
#
# Copyright (c) 2021 Bosch Rexroth AG
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import sys
import logging
from systemd.journal import JournaldLogHandler


def run():
    print("Simple Snap for insert different loglevels with Python")
        
    log = logging.getLogger()
    log.setLevel(logging.DEBUG)
    log.addHandler(JournaldLogHandler())

    log.exception("I am an exception message")
    log.critical("I am a critical message")
    log.error("I am an error")
    log.warning("I am a warning")
    log.info("I am an infomessage")
    log.debug("I am a debugmessage")
    
    #<timestamp>|<userId>|<mainDiagnosisCode>|<mainTitle>|<detailedDiagnosisCode>|<detailedTitle>|<entity>|<dynamicSource>|<dynamicDescription>
 