#-*- coding: utf-8 -*-
##################################
#   BJResAll 文件批处理工具
#
#   创建时间: 2016-04-18
##################################

import os, sys

if len(sys.argv) == 3:
    for dirpath, dirnames, filenames in os.walk("."):
        if len(filenames) == 0:
            continue
        for filename in filenames:
            ext = os.path.splitext(filename)[1]
            if ext != sys.argv[2]:
                continue
            full_path = dirpath + "/" + filename
            if sys.argv[1] == "e":
                os.system("./BJResSig g " + full_path)
                os.system("./BJResEnc " + full_path)
            elif sys.argv[1] == "d":
                os.system("./BJResDec " + full_path)
                os.system("./BJResSig c " + full_path)
            else:
                print full_path
else:
    print "Usage: BJResAll.py <e/d> <.ext>\n"
