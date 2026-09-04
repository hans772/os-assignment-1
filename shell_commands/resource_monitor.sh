ps axo pid,comm,%cpu,%mem | 
awk 'NR == 1 { print $0, "SCORE" } NR > 1 {score = 3 * $(NF-1) + 2 * $NF; print $1, $2, $(NF-1), $(NF), score}' | 
column -t | 
(read -r header; echo "$header"; sort -k5,5rn) | 
head -n 5