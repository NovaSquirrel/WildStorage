/*
Names of all the neighbors in Animal Crossing: Wild World.

By Damian Yerrick. No rights reserved and ABSOLUTELY NO WARRANTY.
Contains portions of a Nintendo copyrighted work; the author believes
that this is a fair use.
*/

#include <sys/types.h>

#define N_NEIGHBOR_NAMES 150

static const char *const acwwNeighborNames[N_NEIGHBOR_NAMES] =
{
 "Cyrano",
 "Antonio",
 "Pango",
 "Anabelle",
 "Teddy",
 "Pinky",
 "Curt",
 "Chow",
 "Jay",
 "Robin",
 "Anchovy",
 "Twiggy",
 "Jitters",
 "Angus",
 "Rodeo",
 "Bob",
 "Mitzi",
 "Rosie",
 "Olivia",
 "Kiki",
 "Tangy",
 "Punchy",
 "Purrl",
 "Moe",
 "Kabuki",
 "Kid Cat",
 "Monique",
 "Tabby",
 "Bluebear",
 "Maple",
 "Poncho",
 "Pudge",
 "Kody",
 "Stitches",
 "Goose",
 "Benedict",
 "Egbert",
 "Patty",
 "Tipper",
 "Alfonso",
 "Alli",
 "Goldie",
 "Butch",
 "Lucky",
 "Biskit",
 "Bones",
 "Portia",
 "Walker",
 "Daisy",
 "Bill",
 "Joey",
 "Pate",
 "Maelle",
 "Deena",
 "Pompom",
 "Mallary",
 "Freckles",
 "Derwin",
 "Drake",
 "Opal",
 "Dizzy",
 "Big Top",
 "Eloise",
 "Margie",
 "Lily",
 "Ribbot",
 "Frobert",
 "Camofrog",
 "Drift",
 "Wart Jr.",
 "Puddles",
 "Jeremiah",
 "Chevre",
 "Nan",
 "Cesar",
 "Peewee",
 "Boone",
 "Rocco",
 "Buck",
 "Victoria",
 "Savannah",
 "Elmer",
 "Roscoe",
 "Yuka",
 "Alice",
 "Melba",
 "Kitt",
 "Mathilda",
 "Bud",
 "Elvis",
 "Dora",
 "Limberg",
 "Bella",
 "Bree",
 "Samson",
 "Rod",
 "Octavian",
 "Marina",
 "Queenie",
 "Gladys",
 "Apollo",
 "Amelia",
 "Pierce",
 "Aurora",
 "Roald",
 "Cube",
 "Hopper",
 "Friga",
 "Gwen",
 "Curly",
 "Truffles",
 "Rasher",
 "Hugh",
 "Lucy",
 "Bunnie",
 "Dotty",
 "Coco",
 "Snake",
 "Gaston",
 "Gabi",
 "Pippy",
 "Tiffany",
 "Genji",
 "Ruby",
 "Tank",
 "Rhonda",
 "Vesta",
 "Baabara",
 "Peanut",
 "Blaire",
 "Filbert",
 "Pecan",
 "Nibbles",
 "Agent S",
 "Caroline",
 "Sally",
 "Static",
 "Mint",
 "Rolf",
 "Rowan",
 "Chief",
 "Lobo",
 "Wolfgang",
 "Whitney",
 "Champ",
 "Nana",
 "Simon",
 "Tammi",
 "Monty",
 "Elise"
};

// 0-149: animals; 255: not present
const char *getNeighborName(size_t id) {
  return (id < N_NEIGHBOR_NAMES) ? acwwNeighborNames[id] : NULL;
}
