#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Rumanita

// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.

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
  const vector<Dir> dirs = {Up,Down,Left,Right};
  typedef vector<Pos> vecPos;
  typedef vector< vecPos > matPos;    //Taulell de posicions
  typedef vector<bool> vecBool;       
  typedef vector< vecBool > matBool;  //Taulell de visitats
  typedef vector<int> vecInt;

  /**
   * Functions
   */

  bool casella_zombie(Pos p) {
    //      pos valida          es carrer             hi ha algu                es zombie
    return pos_ok(p) and cell(p).type == Street and cell(p).id != -1 and unit(cell(p).id).player == -1;
  }

  bool casella_jugador(Pos p) {
    //      pos valida          es carrer             hi ha algu                diff de jo                      diff de mort
    return pos_ok(p) and cell(p).type == Street and cell(p).id != -1 and unit(cell(p).id).player != me() and unit(cell(p).id).type != Dead;
  }

  bool casella_menjar(Pos p) {
    //      pos valida          es carrer            menjar true
    return pos_ok(p) and cell(p).type == Street and cell(p).food ;
  }

  bool casella_conquistar(Pos p) {
    //      pos valida          es carrer            no hi ha ningu           no es d ningu      diff de meva propietat
    return pos_ok(p) and cell(p).type == Street and cell(p).id == -1 and (cell(p).owner == -1 or cell(p).owner != me());
  }

  void move_units() {
    vector<int> U = alive_units(me());
    int n = U.size();
    for (int i = 0; i < n; ++i) {
        int id = U[i];
        Pos inicial = unit(id).pos;
        //cerr << "DIRs " << Dir(0*2) << ' ' << Dir(1*2) << ' ' << Dir(2*2) << ' ' << Dir(3*2) << endl;
        matBool V(60,vecBool(60,false));
        V[inicial.i][inicial.j] = true;
        
        queue< pair <Pos, Dir> > Q;
        Dir f = Down;
        Q.push({inicial,f}); //origen

        bool first = true, found = false, zombie = false, clan = false;
        /*for (int i = 0; i < 4 and not zombie; ++i) {
          //f = dirs[i];
          //cerr << "DIR " << ii << " VEC DIRS[I] " << dirs[i] << endl;
          //if (i == 0) ii = Up;
          if (casella_zombie(inicial+Dir(i*2))) {
          //if (pos_ok(inicial+ii) and cell(inicial).type != Waste and (cell(inicial+ii).id != -1) and (unit(cell(inicial).id).player == -1)) {
            zombie = true;
            f = Dir(i*2);
            cerr << "ENTRA AL 1r IF " << "POS " << inicial+f << " DIR " << f << " pos i j " << inicial.i << ' ' << inicial.j << endl;
          }
        }
        if (zombie) {
          cerr << "ENTRA AL MOVE IF ZOMBIE ID " << id << " DIR " << f << endl;
          move(id,f);
          //move(id,Down);
          cerr << "CELLTYPE(INICIAL+F) " << cell(inicial+f).type << " buida? " << cell(inicial+f).id << endl;
        }
        else {
          */while (not Q.empty() and not found) {
            zombie = false;
            clan = false;

            
            Pos u = Q.front().first;
            Dir d = Q.front().second;

            Q.pop();

            if (casella_jugador(u)) {
            //if (pos_ok(u) and cell(u).type == Street and (cell(inicial+Dir(i)).id != -1) and (unit(cell(inicial).id).player != me())) {
              found = true;
              move(id,Dir(d));
              cerr << "ENTRA 1r IF DEL WHILE" << endl;
            }
            else {
              for (int i = 0; i < 4 and not found; ++i) {
                Dir ii = dirs[i];
                Pos x = u + ii;
                //if (not V[x.i][x.j] and (casella_conquistar(x) or casella_menjar(x))) {
                if (pos_ok(x) and not V[x.i][x.j] and cell(x).type == Street and cell(u).id == -1 and cell(u).owner != me()) {
                  V[x.i][x.j] = true;
                  Q.push({x,ii});
                }
              }
            }
          }
        }
    //}
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
      move_units();
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
