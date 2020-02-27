set xrange [600:1200]
set yrange [1600:1000]

plot -0.595744669*x+1793.9172826080398 title "grad"
replot -0.595744669*x+1882.11865 title "parallel to grad"
replot 1.91011238*x-361.49649483827602 title "last grad"
replot 1.91011238*x-524.86852020953006 title "parallel to last grad"
replot 2.80916023*x-1134.80798 title "before_last"

#plot -0.00622460432*x+1168.78735
#replot 0.150483921*x+748.614502
#replot 0.00245851255*x+1165.58362
replot '-' w p ls 1 title "grad v1" , '-' w p ls 2 title "last grad v1", '-' w p ls 3 title "last grad mid", '-' w p ls 4 title "last grad v2", '-' w p ls 5 title "111 v0", '-' w p ls 6 title "111 v1", '-' w p ls 7 title "111 v2", '-' w p ls 8 title "111 v3"
898.934692 1346.58313
e
927.281006 1246.34241
e
860.15033 1281.4873
e
793.019653 1316.6322
e
790.544678 1311.90479
e
929.745117 1251.04907
e
759.756104 1253.09521
e
960.544556 1309.87939
e
