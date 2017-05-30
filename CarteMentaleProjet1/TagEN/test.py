
import subprocess
#from os import popen
#popen('./tagen :mucfr testTag.txt.')


import os
##subcmd=os.popen('./tagen :mucfr testTag.txt.') #comme pour les fichiers, 'r' ou 'w'
##print (subcmd.read()) #les mêmes méthodes qu'un fichier
##


a=subprocess.Popen(["./tagen", ":mucfr", "testTag.txt"])
#b=subprocess.check_output(["ls", "-la"])

print(a)
