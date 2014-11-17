    tpbconv -s full.tpr -n energy.ndx -o energy.tpr
    mdrun -rerun energy.xtc -s energy.tpr -e energy.edr
    echo -e "\e[3J"    


