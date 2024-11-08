//Nicholas Hoven
//2024

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <cstdlib>
using namespace std;

//Description: This is the actman data structure
//Preconditions: none
//Postcondtitions: none
struct actman{
  char appearance;
  int r;
  int c;
  int score;
  bool has_bullet = true;
  bool is_alive = true;
  vector<char> moves_list;
  int move_count;
};

//Description: This is the data structure for ogres
//Preconditions: none
//Postcondtitions: none
struct ogre{
  char appearance; //this is how the ogre looks on the gameboard
  int r; //these represent position
  int c;
  bool is_alive;

  void kill(){
      appearance = '@';
      is_alive = false;
  }
};

//Description: This is the data structure for demons
//Preconditions: none
//Postcondtitions: none
struct demon{
  char appearance; //this is how the demon looks on the gameboard
  int r; //these represent position
  int c;
  bool is_alive;
  void kill(){
      appearance = '@';
      is_alive = false;
  }
};

//Description: Data structure for actman moves
//Preconditions: none
//Postcondtitions: none
struct Move{
  string direction;
  int r_change;
  int c_change;
  double distance;
};

//Description: Data structure for the board snapshots to be used in the breadth first search tree
//Preconditions: none
//Postcondtitions: none
struct board_snapshot{
    vector<string> board;
    vector<ogre> ogres;
    vector<demon> demons;
    actman act_man;
    board_snapshot* parent;
    vector<board_snapshot *> kids;
    bool used = false;
    int ID;
    int p_cost;
    int parent_ID;
    int calculate_cost(){ //this is used to calulate cost(s)
        return act_man.move_count;
    }
    int calculate_h(){ //used to calculate heristic cost
        return ogres.size() + demons.size();
    }
    int calculate_pcost(){ //used to calculate planned cost
        p_cost = calculate_cost() + calculate_h();
        return calculate_cost() + calculate_h();
    }
};


//Description: Data strucrure for actman move data specifically
//Preconditions: none
//Postcondtitions: none
struct act_move{
    string direction;
    int r_change;
    int c_change;
    char num_direction;
};

//Description: This detects if a given move is valid. Works for actman, demons and ogres
//Preconditions: The board is valid, the given direction is valid
//Postcondtitions: A bool is returned for the validity of the move
bool valid_move(vector<string> board, int r, int c, string direction){ //accepts 8 cardinal directions: n, ne, e, se, s, sw, w, nw
  if(direction == "n"){
    if(board[r-1][c]  == '#'){
      return false;
    }
  }
  if(direction == "ne"){
    if(board[r-1][c+1] == '#'){
      return false;
    }
  }
  if(direction == "e"){
    if(board[r][c+1] == '#'){
      return false;
    }
  }
  if(direction == "se"){
    if(board[r+1][c+1] == '#'){
      return false;
    }
  }
  if(direction == "s"){
    if(board[r+1][c] == '#'){
      return false;
    }
  }
  if(direction == "sw"){
    if(board[r+1][c-1] == '#'){
      return false;
    }
  }
  if(direction == "w"){
    if(board[r][c-1] == '#'){
      return false;
    }
  }
  if(direction == "nw"){
    if(board[r-1][c-1] == '#'){
      return false;
    }
  }
  return true;
}

//Description: This is used to initialize the actman data structure given the board input file
//Preconditions: The starting board is valid and contains actman
//Postcondtitions: The actman struct is intialized with the correct information
void initialize_actman(vector<string> board, actman &act_man){
  act_man.score = 50;
  act_man.has_bullet = true;
  for(int r = 0; r < board.size(); r++){
    for(int c = 0; c < board[r].length(); c++){
      if(board[r][c] == 'A'){
        act_man.r = r;
        act_man.c = c;
        break;
      }
    }
  }
}

//Description: intializes all of the ogres on the gameboard
//Preconditions: the gameboard is valid
//Postcondtitions: The ogres are made and added to the vector
void intialize_ogres(vector<string> board, vector<ogre> &ogres){
   for(int r = 0; r < board.size(); r++){
     for(int c = 0; c < board[r].length(); c++){
        if(board[r][c] == 'G'){
          ogre shrek;
          shrek.r = r;
          shrek.c = c;
          shrek.is_alive = true;
          ogres.push_back(shrek);
        }
     }
   }
   for(int r = 0; r < board.size(); r++){  //intial monsters corpses will be put in as dead ogres
     for(int c = 0; c < board[r].length(); c++){
        if(board[r][c] == '@'){
          ogre shrek;
          shrek.r = r;
          shrek.c = c;
          shrek.is_alive = false;
          ogres.push_back(shrek);
        }
     }
   }
}

//Description: intializes all the ogres on the board
//Preconditions: the board is valid
//Postcondtitions: the demons are made and added to the vector
void intialize_demons(vector<string> board, vector<demon> &demons){
   for(int r = 0; r < board.size(); r++){
     for(int c = 0; c < board[r].length(); c++){
        if(board[r][c] == 'D'){
          demon satan;
          satan.r = r;
          satan.c = c;
          satan.is_alive = true;
          demons.push_back(satan);
        }
     }
   }
}

//Description: This creates the gameboard from the given input file
//Preconditions: the input file is valid
//Postcondtitions: the board is created into a usable vector
void create_game_board(string input_file, vector<string> &board){
  ifstream file(input_file);
  string line;
  while (getline(file, line)) {
    board.push_back(line);
  }
  file.close();
}

//Description: This outputs the board to the console for easy debugging
//Preconditions: none
//Postcondtitions: board is printed
void print_board(vector<string> board, actman act_man){
  cout << "Score: " << act_man.score << "\n";
  for(int i = 0; i < board.size(); i++){
    cout << board[i] << "\n";
  }
}

//Description: This is a simple function that is used to calculate distance to be used with the monsters
//Preconditions: none
//Postcondtitions: distance is returned
double calculate_distance(int r1, int c1, int r2, int c2) {
  int rowDiff = r1 - r2;
  int colDiff = c1 - c2;
  int rowDiffSquared = rowDiff * rowDiff;
  int colDiffSquared = colDiff * colDiff;
  int sumOfSquaredDiffs = rowDiffSquared + colDiffSquared;
  double distance = sqrt(sumOfSquaredDiffs);
  return distance;
}

//Description: this is used to calculate an ogres move
//Preconditions: board, ogre, and actman are valid
//Postcondtitions: ogre is moved on the board
void calculate_ogre_move(vector<string> board, ogre &shrek, actman act){
  vector<Move> move_list;
  Move temp_move;
  if(shrek.is_alive == true){
    if(valid_move(board, shrek.r, shrek.c, "n")){
      temp_move.direction = "n";
      temp_move.r_change = -1;
      temp_move.c_change = 0;
      temp_move.distance = calculate_distance(shrek.r -1, shrek.c, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, shrek.r, shrek.c, "ne")){
      temp_move.direction = "ne";
      temp_move.r_change = -1;
      temp_move.c_change = 1;
      temp_move.distance = calculate_distance(shrek.r -1, shrek.c +1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, shrek.r, shrek.c, "e")){
      temp_move.direction = "e";
      temp_move.r_change = 0;
      temp_move.c_change = 1;
      temp_move.distance = calculate_distance(shrek.r, shrek.c + 1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, shrek.r, shrek.c, "se")){
      temp_move.direction = "se";
      temp_move.r_change = 1;
      temp_move.c_change = 1;
      temp_move.distance = calculate_distance(shrek.r + 1, shrek.c + 1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, shrek.r, shrek.c, "s")){
      temp_move.direction = "s";
      temp_move.r_change = 1;
      temp_move.c_change = 0;
      temp_move.distance = calculate_distance(shrek.r + 1, shrek.c, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, shrek.r, shrek.c, "sw")){
      temp_move.direction = "sw";
      temp_move.r_change = 1;
      temp_move.c_change = -1;
      temp_move.distance = calculate_distance(shrek.r +1, shrek.c -1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, shrek.r, shrek.c, "w")){
      temp_move.direction = "w";
      temp_move.r_change = 0;
      temp_move.c_change = -1;
      temp_move.distance = calculate_distance(shrek.r, shrek.c-1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, shrek.r, shrek.c, "nw")){
      temp_move.direction = "nw";
      temp_move.r_change = -1;
      temp_move.c_change = -1;
      temp_move.distance = calculate_distance(shrek.r -1, shrek.c -1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(move_list.size() != 0){ //if there are possible moves
      double min_distance = move_list[0].distance;
      int min_position = 0;
      for(int i = 0; i < move_list.size(); i++){
        if(move_list[i].distance < min_distance){
          min_distance = move_list[i].distance;
          min_position = i;
        }
      }
      shrek.r = shrek.r + move_list[min_position].r_change;
      shrek.c = shrek.c + move_list[min_position].c_change;
    }
  }
}

//Description: This calculates the demon move
//Preconditions: board, demon, and actman are valid
//Postcondtitions: the demon is moved on the board
void calculate_demon_move(vector<string> board, demon &satan, actman act){
  vector<Move> move_list;
  Move temp_move;
  if(satan.is_alive == true){
    if(valid_move(board, satan.r, satan.c, "n")){
      temp_move.direction = "n";
      temp_move.r_change = -1;
      temp_move.c_change = 0;
      temp_move.distance = calculate_distance(satan.r -1, satan.c, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, satan.r, satan.c, "nw")){
      temp_move.direction = "nw";
      temp_move.r_change = -1;
      temp_move.c_change = -1;
      temp_move.distance = calculate_distance(satan.r -1, satan.c -1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, satan.r, satan.c, "w")){
      temp_move.direction = "w";
      temp_move.r_change = 0;
      temp_move.c_change = -1;
      temp_move.distance = calculate_distance(satan.r, satan.c-1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, satan.r, satan.c, "sw")){
      temp_move.direction = "sw";
      temp_move.r_change = 1;
      temp_move.c_change = -1;
      temp_move.distance = calculate_distance(satan.r +1, satan.c -1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, satan.r, satan.c, "s")){
      temp_move.direction = "s";
      temp_move.r_change = 1;
      temp_move.c_change = 0;
      temp_move.distance = calculate_distance(satan.r + 1, satan.c, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, satan.r, satan.c, "se")){
      temp_move.direction = "se";
      temp_move.r_change = 1;
      temp_move.c_change = 1;
      temp_move.distance = calculate_distance(satan.r + 1, satan.c + 1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, satan.r, satan.c, "e")){
      temp_move.direction = "e";
      temp_move.r_change = 0;
      temp_move.c_change = 1;
      temp_move.distance = calculate_distance(satan.r, satan.c + 1, act.r, act.c);
      move_list.push_back(temp_move);
    }
    if(valid_move(board, satan.r, satan.c, "ne")){
      temp_move.direction = "ne";
      temp_move.r_change = -1;
      temp_move.c_change = 1;
      temp_move.distance = calculate_distance(satan.r -1, satan.c +1, act.r, act.c);
      move_list.push_back(temp_move);
    }

    if(move_list.size() != 0){ //if there are possible moves
      double min_distance = move_list[0].distance;
      int min_position = 0;
      for(int i = 0; i < move_list.size(); i++){
        if(move_list[i].distance < min_distance){
          min_distance = move_list[i].distance;
          min_position = i;
        }
      }
      satan.r = satan.r + move_list[min_position].r_change;
      satan.c = satan.c + move_list[min_position].c_change;
    }
  }
}

//Description: This updates all the recently moved monsters and actman on the board
//Preconditions: none
//Postcondtitions: board is updated
void update_board(vector<string> &board, vector<ogre> ogres, vector<demon> demons, actman act_man){
  for(int r = 0; r < board.size(); r++){
      for(int c = 0; c < board[r].size(); c++){
          if((board[r][c] == 'G')||(board[r][c] == 'D')||board[r][c] == 'A'){
            board[r][c] = ' ';
          }
      }
  }
  if(act_man.is_alive == true){
    board[act_man.r][act_man.c] = 'A';
  }
  if(act_man.is_alive == false){
    board[act_man.r][act_man.c] = 'X';
  }
  for(int i = 0; i < ogres.size(); i++){
      if(ogres[i].is_alive == true){
        board[ogres[i].r][ogres[i].c] = 'G'; //this replaces all of the ogres
      }
      if(ogres[i].is_alive == false){
        board[ogres[i].r][ogres[i].c] = '@'; //this replaces all of the ogres
      }
  }
  for(int i = 0; i < demons.size(); i++){
      if(demons[i].is_alive == true){
        board[demons[i].r][demons[i].c] = 'D'; //this replaces all of the demons
      }
      if(demons[i].is_alive == false){
        board[demons[i].r][demons[i].c] = '@'; //this replaces all of the demons
      }
  }
}

//Description: This checks if any of the monsters collided with eachother or actman
//Preconditions: none
//Postcondtitions: kills all things that collided
void checkCollisions(std::vector<ogre>& ogres, std::vector<demon>& demons, actman& act_man) {
    // Check for collisions with Act-Man
    for (ogre& o : ogres) {
        if (o.r == act_man.r && o.c == act_man.c) {
            o.is_alive = false;
            o.appearance = '@'; // Change appearance to '@' if Ogre is dead
            act_man.is_alive = false;
            act_man.score = 0; //actman died
            return; // No need to continue checking if Act-Man is already dead
        }
    }
    for (demon& d : demons) {
        if (d.r == act_man.r && d.c == act_man.c) {
            d.is_alive = false;
            d.appearance = '@'; // Change appearance to '@' if Demon is dead
            act_man.is_alive = false;
            act_man.score = 0; //actman died
            return; // No need to continue checking if Act-Man is already dead
        }
    }

    // Check for collisions among Ogres
    for (size_t i = 0; i < ogres.size(); ++i) {
        //if (!ogres[i].is_alive) continue; // Skip if ogre is dead
        for (size_t j = i + 1; j < ogres.size(); ++j) {
            if (j >= ogres.size()) continue; // Bounds check
              if((ogres[i].is_alive == true)||(ogres[j].is_alive == true)){
                if (ogres[i].r == ogres[j].r && ogres[i].c == ogres[j].c) {
                  ogres[i].is_alive = false;
                  act_man.score = act_man.score + 5;
                  ogres[j].is_alive = false;
                  act_man.score = act_man.score + 5;
                  ogres[i].appearance = '@'; // Change appearance to '@' if Ogre is dead
                  ogres[j].appearance = '@'; // Change appearance to '@' if Ogre is dead
              }
          }
        }
    }

    // Check for collisions among Demons
    for (size_t i = 0; i < demons.size(); ++i) {
        //if (!demons[i].is_alive) continue; // Skip if demon is dead
        for (size_t j = i + 1; j < demons.size(); ++j) {
            if (j >= demons.size()) continue; // Bounds check
            if((demons[i].is_alive == true)||(demons[j].is_alive == true)){
            if (demons[i].r == demons[j].r && demons[i].c == demons[j].c) {
                demons[i].is_alive = false;
                act_man.score = act_man.score + 5;
                demons[j].is_alive = false;
                act_man.score = act_man.score + 5;
                demons[i].appearance = '@'; // Change appearance to '@' if Demon is dead
                demons[j].appearance = '@'; // Change appearance to '@' if Demon is dead
            }
            }
        }
    }

    // Check for collisions between Demons and Ogres
    for (size_t i = 0; i < demons.size(); ++i) {
        //if (!demons[i].is_alive) continue; // Skip if demon is dead
        for (size_t j = 0; j < ogres.size(); ++j) {
            if (j >= ogres.size()) continue; // Bounds check
            if((demons[i].is_alive == true)||(ogres[j].is_alive == true)){
            if (demons[i].r == ogres[j].r && demons[i].c == ogres[j].c) {
                demons[i].is_alive = false;
                act_man.score = act_man.score + 5;
                ogres[j].is_alive = false;
                act_man.score = act_man.score + 5;
                demons[i].appearance = '@'; // Change appearance to '@' if Demon is dead
                ogres[j].appearance = '@'; // Change appearance to '@' if Ogre is dead
            }
            }
        }
    }
}

//Description: This shoots the actman bullet in a random direction
//Preconditions: none
//Postcondtitions: the bullet is shot and things are killed
void shoot_bullet(vector<string> &board, actman &act_man, vector<ogre> &ogres, vector<demon> &demons, int direction){
    cout << "BULLET SHOT!" << "\n";
    act_man.has_bullet = false;
    //int random_direction = rand() % 4;
    int random_direction = direction;
    if(random_direction == 0){ //case north
      //actman_move.push_back('N');
      for(int i = 1; i < 50; i++){
        if(board[act_man.r - i][act_man.c] == '#'){ //case wall hit
            break;
        }
        if(board[act_man.r - i][act_man.c] == 'G'){ //case hit ogre
            for(int g = 0; g < ogres.size(); g++){
                if((ogres[g].r == act_man.r - i)&&(ogres[g].c == act_man.c)){
                    ogres[g].kill();
                    act_man.score = act_man.score + 5;
                }
            }
        }
        if(board[act_man.r - i][act_man.c] == 'D'){ //case hit demon
            for(int d = 0; d < demons.size(); d++){
                if((demons[d].r == act_man.r - i)&&(demons[d].c == act_man.c)){
                    demons[d].kill();
                    act_man.score = act_man.score + 5;
                }
            }
        }
      }
    }
    if(random_direction == 1){ //case east
      //actman_move.push_back('E');
      for(int i = 1; i < 50; i++){
        if(board[act_man.r][act_man.c + i] == '#'){ //case wall hit
            break;
        }
        if(board[act_man.r][act_man.c + i] == 'G'){ //case hit ogre
            for(int g = 0; g < ogres.size(); g++){
                if((ogres[g].r == act_man.r)&&(ogres[g].c == act_man.c + i)){
                    ogres[g].kill();
                    act_man.score = act_man.score + 5;
                }
            }
        }
        if(board[act_man.r][act_man.c + i] == 'D'){ //case hit demon
            for(int d = 0; d < demons.size(); d++){
                if((demons[d].r == act_man.r)&&(demons[d].c == act_man.c + i)){
                    demons[d].kill();
                    act_man.score = act_man.score + 5;
                }
            }
        }
      }
    }
    if(random_direction == 2){ //case south
      //actman_move.push_back('S');
      for(int i = 1; i < 50; i++){
        if(board[act_man.r + i][act_man.c] == '#'){ //case wall hit
            break;
        }
        if(board[act_man.r + i][act_man.c] == 'G'){ //case hit ogre
            for(int g = 0; g < ogres.size(); g++){
                if((ogres[g].r == act_man.r + i)&&(ogres[g].c == act_man.c)){
                    ogres[g].kill();
                    act_man.score = act_man.score + 5;
                }
            }
        }
        if(board[act_man.r + i][act_man.c] == 'D'){ //case hit demon
            for(int d = 0; d < demons.size(); d++){
                if((demons[d].r == act_man.r + i)&&(demons[d].c == act_man.c)){
                    demons[d].kill();
                    act_man.score = act_man.score + 5;
                }
            }
        }
      }
    }
    if(random_direction == 3){ //case west
      //actman_move.push_back('W');
      for(int i = 1; i < 50; i++){
        if(board[act_man.r][act_man.c - i] == '#'){ //case wall hit
            break;
        }
        if(board[act_man.r][act_man.c - i] == 'G'){ //case hit ogre
            for(int g = 0; g < ogres.size(); g++){
                if((ogres[g].r == act_man.r)&&(ogres[g].c == act_man.c - i)){
                    ogres[g].kill();
                    act_man.score = act_man.score + 5;
                }
            }
        }
        if(board[act_man.r][act_man.c - i] == 'D'){ //case hit demon
            for(int d = 0; d < demons.size(); d++){
                if((demons[d].r == act_man.r)&&(demons[d].c == act_man.c - i)){
                    demons[d].kill();
                    act_man.score = act_man.score + 5;
                }
            }
        }
      }
    }
    update_board(board, ogres, demons, act_man);

}

//Description: This is used to print the results to the given output file
//Preconditions: none
//Postcondtitions: the output file is opened, written to, and closed
void create_output_file(vector<string> board, actman act_man, vector<char> actman_moves, string output_file) {
    ofstream outfile(output_file);
    for(int i = 0; i < actman_moves.size(); i++) {
        outfile << actman_moves[i];
    }
    outfile << "\n";
    outfile << act_man.score << "\n";
    for(int i = 1; i < board.size(); i++) {
        if(board[i] == ""){
            break;
        }
        outfile << board[i] << "\n";
    }
    outfile.close();
}

//Description: This is a function to help determine if a win for actman has been found
//Precondition: none
//Postconditions: outputs true or false for if a win has been found
bool search_board_for_win(vector<string> board){
    cout << "SEARCHING!" << "\n";
    for(int r = 0; r < board.size(); r++){
        for(int c = 0; c < board[r].size(); c++){
            cout << board[r][c];
        }
        cout << "\n";
    }
    cout << "\n";
    bool found_a = false;
    bool found_m = false;
    for(int r = 0; r < board.size(); r++){
        for(int c = 0; c < board[r].size(); c++){
            if(board[r][c] == 'A'){
                found_a = true;
            }
            if((board[r][c] == 'G')||(board[r][c] == 'D')){
                found_m = true;
            }
        }
    }
    if((found_a == true)&&(found_m ==false)){
        cout << "KILLED THEM ALL!" << "\n";
        return true;
    }else{
        return false;
    }
}

//Description: this parses through all of the possible states
//Preconditions: none
//Postconditions: the Iterative deepeing depth first search is completed and true or false is returned if a victory state is found
bool actman_move_search(board_snapshot &start_board, vector<board_snapshot> &kids, string out_file){
    //bool make_snapshot = false;
    //start_board.used = true;
    if((valid_move(start_board.board, start_board.act_man.r, start_board.act_man.c, "n"))){
        board_snapshot temp = start_board;
        temp.kids.clear();
        cout << "NORTH" << "\n";
        temp.act_man.move_count += 1;
        temp.act_man.score -= 1;
        temp.act_man.moves_list.push_back('8');
        temp.act_man.r = temp.act_man.r - 1;
        for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
          calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
        }
        for(int i = 0; i < temp.demons.size(); i++){
          calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
        }
        checkCollisions(temp.ogres, temp.demons, temp.act_man);
        update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
        if(search_board_for_win(temp.board)){
            create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
            return true;
        }
        if(temp.act_man.is_alive == true){
            kids.push_back(temp);
        }
     }
     if((valid_move(start_board.board, start_board.act_man.r, start_board.act_man.c, "ne"))){
         board_snapshot temp = start_board;
         temp.kids.clear();
         cout << "NORTHEAST" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 1;
         temp.act_man.moves_list.push_back('9');
         temp.act_man.r -= 1;
         temp.act_man.c += 1;
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if((valid_move(start_board.board, start_board.act_man.r, start_board.act_man.c, "e"))){
         board_snapshot temp = start_board;
         temp.kids.clear();
         cout << "EAST" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 1;
         temp.act_man.moves_list.push_back('6');
         temp.act_man.c += 1;
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if((valid_move(start_board.board, start_board.act_man.r, start_board.act_man.c, "se"))){
         board_snapshot temp = start_board;
         temp.kids.clear();
         cout << "SOUTHEAST" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 1;
         temp.act_man.moves_list.push_back('3');
         temp.act_man.r += 1;
         temp.act_man.c += 1;
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if((valid_move(start_board.board, start_board.act_man.r, start_board.act_man.c, "s"))){
         board_snapshot temp = start_board;
         temp.kids.clear();
         cout << "SOUTH" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 1;
         temp.act_man.moves_list.push_back('2');
         temp.act_man.r += 1;
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if((valid_move(start_board.board, start_board.act_man.r, start_board.act_man.c, "sw"))){
         board_snapshot temp = start_board;
         temp.kids.clear();
         cout << "SOUTHWEST" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 1;
         temp.act_man.moves_list.push_back('1');
         temp.act_man.r += 1;
         temp.act_man.c -= 1;
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if((valid_move(start_board.board, start_board.act_man.r, start_board.act_man.c, "w"))){
         board_snapshot temp = start_board;
         temp.kids.clear();
         cout << "WEST" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 1;
         temp.act_man.moves_list.push_back('4');
         temp.act_man.c -= 1;
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if((valid_move(start_board.board, start_board.act_man.r, start_board.act_man.c, "nw"))){
         board_snapshot temp = start_board;
         temp.kids.clear();
         cout << "NORTHWEST" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 1;
         temp.act_man.moves_list.push_back('7');
         temp.act_man.r -= 1;
         temp.act_man.c -= 1;
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if(start_board.act_man.has_bullet == true){
         board_snapshot temp = start_board;
         cout << "Shoot NORTH" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 20;
         temp.act_man.moves_list.push_back('N');
         temp.act_man.has_bullet = false;
         shoot_bullet(temp.board, temp.act_man, temp.ogres, temp.demons, 0);
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }

     }
     if(start_board.act_man.has_bullet == true){
         board_snapshot temp = start_board;
         cout << "Shoot EAST" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 20;
         temp.act_man.moves_list.push_back('E');
         temp.act_man.has_bullet = false;
         shoot_bullet(temp.board, temp.act_man, temp.ogres, temp.demons, 1);
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if(start_board.act_man.has_bullet == true){
         board_snapshot temp = start_board;
         cout << "Shoot SOUTH" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 20;
         temp.act_man.moves_list.push_back('S');
         temp.act_man.has_bullet = false;
         shoot_bullet(temp.board, temp.act_man, temp.ogres, temp.demons, 2);
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     if(start_board.act_man.has_bullet == true){
         board_snapshot temp =  start_board;
         cout << "Shoot WEST" << "\n";
         temp.act_man.move_count += 1;
         temp.act_man.score -= 20;
         temp.act_man.moves_list.push_back('W');
         temp.act_man.has_bullet = false;
         shoot_bullet(temp.board, temp.act_man, temp.ogres, temp.demons, 3);
         for(int i = 0; i < temp.ogres.size(); i++){ //move all ogres
           calculate_ogre_move(temp.board, temp.ogres[i], temp.act_man);
         }
         for(int i = 0; i < temp.demons.size(); i++){
           calculate_demon_move(temp.board, temp.demons[i], temp.act_man);
         }
         checkCollisions(temp.ogres, temp.demons, temp.act_man);
         update_board(temp.board, temp.ogres, temp.demons, temp.act_man);
         if(search_board_for_win(temp.board)){
             create_output_file(temp.board, temp.act_man, temp.act_man.moves_list, out_file);
             return true;
         }
         if(temp.act_man.is_alive == true){
             kids.push_back(temp);
         }
     }
     return false;
}

//Description: This is the sort function that I use to sort the frontier
//Preconditions: vec is not empty
//Postconditions: the vector is sorted from least to most.
void bubbleSort(vector<board_snapshot> &vec) {
    int n = vec.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (vec[j].calculate_pcost() > vec[j + 1].calculate_pcost()) {
                // Swap elements
                std::swap(vec[j], vec[j + 1]);
            }
        }
    }
}

//Description: This is the main function of the program
//Preconditions: none
//Postcondtitions: program is run and completed
int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <input_file_name> <output_file_name>" << endl;
        return 1;
    }

  string input_file_name = argv[1];
  string output_file_name = argv[2];
  actman act_man;
  act_man.is_alive = true;
  vector<ogre> ogres;
  vector<demon> demons;
  vector<string> gameboard;
  vector<char> actman_moves; //this records all of actmans moves
  board_snapshot god;
  vector<board_snapshot> trees;
  create_game_board(input_file_name, god.board);
  initialize_actman(god.board, act_man);
  intialize_ogres(god.board, ogres);
  intialize_demons(god.board, demons);
  god.act_man = act_man;
  god.act_man.move_count = 0;
  god.demons = demons;
  god.ogres = ogres;
  bool broke = false;
  board_snapshot win_condtion;
  vector<board_snapshot> frontier; //THIS IS THE FRONTIER QUEUE
  vector<board_snapshot> temp;

  actman_move_search(god, frontier, output_file_name); //1
  for(int r = 0; r < 50; r++){
    bubbleSort(frontier);
    int len = frontier.size();
    cout << "FRONTIER LEN: " << len << "\n";
    for(int i = 0; i < len; i++){ //2
        if(actman_move_search(frontier[i], temp, output_file_name)){
            cout << "VICTORY!" << "\n";
            broke = true;
            break;
        }
    }
    if(broke){
        break;
    }
    frontier.clear();
    frontier = temp;
    temp.clear();
  }
  cout << "FRONTIER LENGTH: " << frontier.size() << "\n";


  return 0;
}


