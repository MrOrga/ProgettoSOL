#!/bin/bash
IFS='#'
while read -r cut n_test cut n_succ cut n_fail cut
do
	if [ $n_test = 1 ]
	then
		((client++))
		((op+=$n_succ + $n_fail))
		((test1s+=$n_succ))
		((test1f+=$n_fail))
	elif [ $n_test = 2 ];
	then
		((client++));
		((op+=$n_succ + $n_fail))
		((test2s+=$n_succ))
		((test2f+=$n_fail))
	elif [ $n_test = 3 ];
	then
		((client++))
		((op+=$n_succ + $n_fail))
		((test3s+=$n_succ))
		((test3f+=$n_fail))
	fi
done < testout.log

echo	"Client Eseguiti" "$client"
echo	"Operazioni Eseguite" "$op"
echo	"OPERAZIONI DI TEST1 eseguite con successo" "$test1s"
echo	"OPERAZIONI DI TEST1 fallite--------------" "$test1f"
echo	"OPERAZIONI DI TEST2 eseguite con successo" "$test2s"
echo	"OPERAZIONI DI TEST2 fallite--------------" "$test2f"
echo	"OPERAZIONI DI TEST3 eseguite con successo" "$test3s"
echo	"OPERAZIONI DI TEST3 fallite--------------" "$test3f"
