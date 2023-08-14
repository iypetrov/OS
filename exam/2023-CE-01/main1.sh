# Напишете серия от команди, която извежда пътищата до всички файлове (из цялата файлова
# система), чиито собственик е текущият потребител и имат име, завършващо на .blend<число>.
# Примерен изход:
# /home/students/ivan/scenes/landscape.blend1
# /home/students/ivan/scenes/landscape.blend26
# /home/students/ivan/scenes/landscape.blend42
# /home/students/ivan/.local/share/Trash/lewd.blend1
# /tmp/baba.blend5

#!/bin/bash

find . -type f -uid $(id -u) -regex '.*\.blend[0-9]+$'

