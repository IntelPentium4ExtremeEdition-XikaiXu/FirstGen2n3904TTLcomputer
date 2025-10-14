/*This is the gate we will have to use to build by the 2n3904 ttl circuit
 *
 *
 */

void nor_gate(bool &y, bool &a, bool &b);
void nor_gate(bool &y, bool &a, bool &b){
    *y = ~(*a || *b);
}
