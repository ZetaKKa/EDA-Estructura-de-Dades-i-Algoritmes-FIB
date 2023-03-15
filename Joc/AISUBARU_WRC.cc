#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME SUBARU_WRC



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

   
   
   typedef vector < int > VI; //vector ints
   typedef vector < bool > VB; //vector bools
   typedef vector < Pos > VP; //vector Pos
   typedef vector < Dir > VD; //vector Dirs

   typedef vector < VI > MI; //matriu ints
   typedef vector < VB > MB; //matriu bools

   const VD dirs = {Down,Right,Up,Left}; //vector direccions

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
        //if ((cell(p).id != -1 and unit(cell(p).id).type == Dead)) return false;
        //else 
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
        //       no te propietari              
        return cell(p).owner == -1;
    }

    bool prioritat_propia(Pos p) {
        //casella valida checked
        //          propietari                 
        return cell(p).owner == me(); 
    }

    bool menjar_proxim(const Pos& p, Dir& d) {
        for (int i = 0; i < 4; ++i) {
            if (casella_menjar(p+Dir(i*2))) {
                d = Dir(i*2);
                return true;
            }
        }
        return false;
    }

    int prioritats_Dijkstra(Pos p) {
        if (prioritat_enemic(p)) return 1; 
        else if (prioritat_conQueer(p)) return 2;
        else if (prioritat_propia(p)) return 3;
        else {
            //PD: mai entra
            cerr << endl << "ENTRAAAAAAAAAAAAAAAAAAA" << endl << endl << endl;
            return 3;
        }
    }

  /**
   * FUNCIONS DE MOVIMENT
   */

    /**
     * Pre: Matriu de visitats (modificada o no (inici) per la priority_queue)
     * Post: Matriu de visitats amb les caselles Waste i Dead marcades com a visitades
     * Sintesi: Aquesta implementació no millora el winrate, obte un 2% menys aproximadament (implementar millor)
     */
    /*
    void llegir_mapa(MB& V) {
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
    */

   /**
     * Pre: --
     * Post: Dijkstra amb prioritats de 1-menjar, buscar menjar amb prioritats en 1-zones enemigues i 2-zones lliures i 3-zones propies
     * Sintesi: DIJKSTRA => (BUSCAR MENJAR)
     */
    /*
    void busca_menjar(MB& V, const Pos& p_ini, int id) {
            V[p_ini.i][p_ini.j] = true;

            
            priority_queue < pair < int,pair < Pos,Dir > >, vector < pair < int,pair < Pos,Dir > > >, greater < pair < int,pair < Pos,Dir > > > > Q;
            Dir d_ini = Down; //random
            
            Q.push({0,{p_ini,d_ini}});
            
            //      primer                  menjar                  enemic
            bool first = true, gasofa = false, diesel = false, enemic = false;
            
            //else {
                while (not Q.empty() and not gasofa and not diesel and not enemic) {
                    Pos p_act = Q.top().second.first;
                    Dir d_act = Q.top().second.second;
                    int dist_act = Q.top().first;
                    Q.pop();
                    
                    if (not enemic and casella_menjar(p_act)) {
                        //cerr << "ENTRA" << endl;
                        move(id,d_act);
                        diesel = true;
                    }
                    
                    else {
                        for (int i = 0; i < 4 and not diesel and not enemic; ++i) {
                            Pos p_nova = p_act + Dir(i*2); //i*2 = {0->Down,2->Right,4->Up,6->Left}
                            int dist_nova = dist_act;
                            //if (mata_zombies(p_nova) and not V[p_nova.i][p_nova.j]) {
                            if (casella_valida(p_nova) and not V[p_nova.i][p_nova.j]) {
                                
                                
                                if (casella_menjar(p_nova)) dist_nova = 0;
                                else if (prioritat_conQueer(p_nova)) dist_nova += 2; 
                                else if (prioritat_enemic(p_nova)) dist_nova += 1;
                                else if (prioritat_propia(p_nova)) dist_nova += 3;
                                else {
                                    cerr << endl << "ENTRAAAAAAAAAAAAAAAAAAA" << endl << endl << endl;
                                }
                                //cuadrar posicions
                                if (first) d_act = dirs[i];

                                V[p_nova.i][p_nova.j] = true;
                                //cerr << "NEWDIR " << newdir << " DIR(i*2) " << Dir(i*2) << endl;
                                Q.push({dist_nova,{p_nova,d_act}});
                            }
                        }
                        first = false;
                    }
                }
            //}
         }
   */

    
    /**   
     * Pre: --
     * Post: Dijkstra amb prioritats de 1-conquerir i 2-menjar i buscar conquerir amb prioritats en 1-zones enemigues i 2-zones lliures i 3-zones propies
     * Sintesi: DIJKSTRA => (CONQUERIR)
     */
    /*
        void busca_conqueer(MB& V, const Pos& p_ini, int id) {
            V[p_ini.i][p_ini.j] = true;

            
            priority_queue < pair < int,pair < Pos,Dir > >, vector < pair < int,pair < Pos,Dir > > >, greater < pair < int,pair < Pos,Dir > > > > Q;
            Dir d_ini = Down; //random
            
            Q.push({0,{p_ini,d_ini}});
            
            //      primer                  menjar          enemic
            bool first = true, gasofa = false, diesel = false, enemic = false;

            //else {
                while (not Q.empty() and not gasofa and not diesel and not enemic) {
                    Pos p_act = Q.top().second.first;
                    Dir d_act = Q.top().second.second;
                    int dist_act = Q.top().first;
                    Q.pop();
                    
                    //mata zombies
                    if (prioritat_enemic(p_act) or prioritat_conQueer(p_act)) {
                        enemic = true;
                        move(id,d_act);
                    }
                    //menjar
                    else if (not enemic and casella_menjar(p_act)) {
                        //cerr << "ENTRA" << endl;
                        move(id,d_act);
                        diesel = true;
                    }
                    
                    else {
                        for (int i = 0; i < 4 and not diesel and not enemic; ++i) {
                            Pos p_nova = p_act + Dir(i*2); //i*2 = {0->Down,2->Right,4->Up,6->Left}
                            int dist_nova = dist_act;
                            //if (mata_zombies(p_nova) and not V[p_nova.i][p_nova.j]) {
                            if (casella_valida(p_nova) and not V[p_nova.i][p_nova.j]) {
                                
                                
                                if (casella_menjar(p_nova)) dist_nova += 3;
                                else if (prioritat_conQueer(p_nova)) dist_nova += 1; 
                                else if (prioritat_enemic(p_nova)) dist_nova = 0;
                                else if (prioritat_propia(p_nova)) dist_nova += 3;
                                else {
                                    cerr << endl << "ENTRAAAAAAAAAAAAAAAAAAA" << endl << endl << endl;
                                }
                                //cuadrar posicions
                                if (first) d_act = dirs[i];

                                V[p_nova.i][p_nova.j] = true;
                                //cerr << "NEWDIR " << newdir << " DIR(i*2) " << Dir(i*2) << endl;
                                Q.push({dist_nova,{p_nova,d_act}});
                            }
                        }
                        first = false;
                    }
                }
            //}
        
    }
    
    */


    /**
     * Pre: --
     * Post: Dijkstra amb prioritats de 1-menjar i 2-matar zombies i buscar menjar amb prioritats en 1-zones enemigues i 2-zones lliures i 3-zones propies
     * Sintesi: DIJKSTRA => (BUSCAR MENJAR + MATAR + CONQUERIR)
     */
    void play_subaru_impreza_wrx_sti() {

        VI U = alive_units(me());
        int n = U.size();
        for (int i = 0; i < n; ++i){
            int id = U[i];
            Pos p_ini = unit(id).pos;
            
            MB V(60,VB(60,false)); //Matriu visitats
            //llegir_mapa(V);
            V[p_ini.i][p_ini.j] = true;
            
            
            priority_queue < pair < int,pair < Pos,Dir > >, vector < pair < int,pair < Pos,Dir > > >, greater < pair < int,pair < Pos,Dir > > > > Q;
            Dir d_ini = Down; //random
            
            Q.push({0,{p_ini,d_ini}});
            
            //      primer    menjar pel subaru     enemic
            bool first = true, diesel = false, enemic = false;
            
            if (menjar_proxim(p_ini,d_ini)) move(id,d_ini);
            else {
                //      cua no buida   not menjar       not enemic
                while (not Q.empty() and not diesel and not enemic) {
                    Pos p_act = Q.top().second.first;
                    Dir d_act = Q.top().second.second;
                    int dist_act = Q.top().first;
                    Q.pop();
                    
                    //matar zombies
                    if (mata_zombies(p_act)) {
                        enemic = true;
                        move(id,d_act);
                    }
                    //menjar
                    else if (not enemic and casella_menjar(p_act)) {
                        //cerr << "ENTRA" << endl;
                        move(id,d_act);
                        diesel = true;
                    }
                    
                    else {
                        //              direccions not menjar2   not enemic
                        for (int i = 0; i < 4 and not diesel and not enemic; ++i) {
                            //i*2 = {0->Down,2->Right,4->Up,6->Left}
                            Pos p_nova = p_act + Dir(i*2); 
                            int dist_nova = dist_act;

                            //if (mata_zombies(p_nova) and not V[p_nova.i][p_nova.j]) {
                            if (casella_valida(p_nova) and not V[p_nova.i][p_nova.j] /*and cell(p_nova).owner != me()*/) {
                                
                                /*
                                 * Prioritats Dijkstra
                                 * 1->menjar
                                 * 2->casella enemic, 3-> casella buida, 4-> casella propia
                                 */
                                if (casella_menjar(p_nova)) dist_nova = 0;
                                else dist_nova += prioritats_Dijkstra(p_nova);
                                
                                //cuadrar posicions sino no va b
                                if (first) d_act = dirs[i];

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

    play_subaru_impreza_wrx_sti();
    
    /*
     * NO UTILITZAR
     * La millora d'aquest codi és inferior
     *

    bool turbo_stutututu() {
        for (int i = 0; i < 4; ++i) {
            if (i != me()) return score(i) > score(me());
        }
        return false;
    }

    VI U = alive_units(me());
        int n = U.size();
        for (int i = 0; i < n; ++i){
            int id = U[i];
            Pos p_ini = unit(id).pos;
            Dir d_ini = Down;

            MB V(60,VB(60,false)); //Matriu visitats
            V[p_ini.i][p_ini.j] = true;
            if (round() == 0) llegir_mapa(V);

            bool Z = false;
            if (not turbo_stutututu() or round() < 150) {
            //if (score(me()) < 11000) {

                for (int i = 0; i < 4 and not Z; ++i) {
                    if (casella_menjar(p_ini+Dir(i*2))) {
                        Z = true;
                        d_ini = Dir(i*2);
                    }
                }
                if (Z) {
                    Z = false;
                    move(id,d_ini);
                    V[p_ini.i][p_ini.j] = true;
                }
                else busca_menjar(V,p_ini,id);

            }
            else {
                for (int i = 0; i < 4 and not Z; ++i) {
                    if (pos_ok(p_ini+Dir(i*2)) and (prioritat_enemic(p_ini+Dir(i*2)) or prioritat_conQueer(p_ini+Dir(i*2)))) {
                        Z = true;
                        d_ini = Dir(i*2);
                    }
                }
                if (Z) {
                    Z = false;
                    move(id,d_ini);
                    V[p_ini.i][p_ini.j] = true;
                }
                else busca_conqueer(V,p_ini,id);

            }
        }
    }
    
    */
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
