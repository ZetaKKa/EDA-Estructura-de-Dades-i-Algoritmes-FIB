#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME SUBARU_WRC_STI



struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  /**
   * FUNCIONS DE COMPROVACIÓ DE CASELLA
   */

    bool mata_zombies(Pos p) {
        //      pos valida          es carrer               hi ha algu              es zombie
        return pos_ok(p) and cell(p).type == Street and cell(p).id != -1 and unit(cell(p).id).type == Zombie;
    }

    bool mata_alive(Pos p) {
        //      pos valida          es carrer               hi ha algu              not me                                     es alive
        return pos_ok(p) and cell(p).type == Street and cell(p).id != -1 and unit(cell(p).id).player != me() and unit(cell(p).id).type == Alive;
    }

    bool killer(Pos p) {
        //       pos valida         es carrer               hi ha algu              not me                         es zombie o es alive                
        return pos_ok(p) and cell(p).type == Street and cell(p).id != -1 and unit(cell(p).id).player != me() and unit(cell(p).id).type != Dead;
    }

    bool con_Queer(Pos p) {
        //      pos valida          es carrer               not propietari           (no hi ha algu) o    (hi ha algu    i    es zombie o es alive)
        return pos_ok(p) and cell(p).type == Street and cell(p).owner != me() and (cell(p).id == -1 or (cell(p).id != -1 and unit(cell(p).id).type != Dead));
    }

    bool casella_valida(Pos p) {
        //      pos valida          es carrer
        return pos_ok(p) and cell(p).type == Street;
    }
    
    bool casella_menjar(Pos p) {
        //      pos valida          es carrer              menjar
        return pos_ok(p) and cell(p).type == Street and cell(p).food;
    }

    bool prioritat_enemic(Pos p) {
        //casella valida checked
        //        not propietari                es d algu             
        return (cell(p).owner != me() and cell(p).owner != -1);
    }

    bool prioritat_conQueer(Pos p) {
        //casella valida checked
        //         esta buida              
        return cell(p).owner == -1;
    }

    bool prioritat_propia(Pos p) {
        //casella valida checked
        //          propietari                 
        return cell(p).owner == me(); 
    }

  /**
   * FUNCIONS DE MOVIMENT
   */

    /*
     * Pre: Matriu de visitats (modificada o no (inici) per la priority_queue)
     * Post: Matriu de visitats amb les caselles Waste i Dead marcades com a visitades
     */
    void llegir_mapa(vector<vector<bool>>& V) {
        for (int i = 0; i < 60; ++i) {
            for (int j = 0; j < 60; ++j) {
                if (not V[i][j]) {
                    //              es brossa
                    if (cell(Pos(i,j)).type == Waste) V[i][j] = true;
                    //                      hi ha algu                           mort
                    else if (((cell(Pos(i,j)).id != -1 and unit(cell(Pos(i,j)).id).type == Dead))) V[i][j] = true;
                }
                else {
                    //   pos valida i es carrer            no hi ha algu
                    if (casella_valida(Pos(i,j)) and cell(Pos(i,j)).id == -1) V[i][j] = false;
                }
            }
        }
    }
    

    void move_alives() {
        vector<int> U = alive_units(me());
        int n = U.size();
        for (int i = 0; i < n; ++i){
            int id = U[i];
            Pos p_ini = unit(id).pos;
            
            vector <vector <bool> > V(60,vector<bool>(60,false)); //Matriu visitats
            llegir_mapa(V);
            V[p_ini.i][p_ini.j] = true;
            
            
            priority_queue < pair < int,pair < Pos,Dir > >, vector < pair < int,pair < Pos,Dir > > >, greater < pair < int,pair < Pos,Dir > > > > Q;
            Dir d_ini = Down; //random
            
            Q.push({0,{p_ini,d_ini}});
            
            //      primer       zombie      trobat          enemic
            bool first = true, Z = false, found = false, enemic = false;
            
            //casella menjar per evitar Dijkstra
            for (int i = 0; i < 4 and not Z; ++i) {
                if (casella_menjar(p_ini+Dir(i*2))) {
                    Z = true;
                    d_ini = Dir(i*2);
                }
            }
            if (Z) {
                move(id,d_ini);
            }
            
            /*
            else if (score(me()) > 10000) {
                for (int i = 0; i < 4 and not Z; ++i) {
                   if (killer(p_ini+Dir(i*2))) {
                    Z = true;
                    d_ini = Dir(i*2);
                }
                move(id,d_ini);
            }
            } 
            */
            else {
                while (not Q.empty() and not Z and not found and not enemic) {
                    Pos p_act = Q.top().second.first;
                    Dir d_act = Q.top().second.second;
                    int dist_act = Q.top().first;
                    Q.pop();
                    
                    //mata zombies
                    if (mata_zombies(p_act)) {
                        enemic = true;
                        move(id,d_act);
                    }
                    //menjar
                    else if (not enemic and casella_menjar(p_act)) {
                        //cerr << "ENTRA" << endl;
                        move(id,d_act);
                        found = true;
                    }
                    
                    else {
                        for (int i = 0; i < 4 and not found and not enemic; ++i) {
                            Pos p_nova = p_act + Dir(i*2); //i*2 = {0->Down,2->Right,4->Up,6->Left}
                            int dist_nova = dist_act;
                            //if (mata_zombies(p_nova) and not V[p_nova.i][p_nova.j]) {
                            if (casella_valida(p_nova) and not V[p_nova.i][p_nova.j] /*and cell(p_nova).owner != me()*/) {
                                
                                
                                if (casella_menjar(p_nova)) dist_nova = 0;
                                else if (prioritat_conQueer(p_nova)) dist_nova += 2; 
                                else if (prioritat_enemic(p_nova)) ++dist_nova;
                                else if (prioritat_propia(p_nova)) dist_nova += 3;
                                else {
                                    cerr << endl << "ENTRAAAAAAAAAAAAAAAAAAA" << endl << endl << endl;
                                }
                                //cuadrar posicions
                                if (first) {
                                    if (i == 0) d_act = Down;
                                    else if (i == 1) d_act = Right;
                                    else if (i == 2) d_act = Up;
                                    else if (i == 3) d_act = Left;
                                }
                                V[p_nova.i][p_nova.j] = true;
                                //cerr << "NEWDIR " << newdir << " DIR(i*2) " << Dir(i*2) << endl;
                                Q.push({dist_nova,{p_nova,d_act}});
                            }
                        }
                        first = false;
                    }
                }
            } 
        }
    }

  /**
   * Play method, invoked once per each round.
   * Main del jugador
   */

  virtual void play () {
    move_alives();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
