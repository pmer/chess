#include "client.h"

#include "curl.h"

#include "os/os.h"
#include "util/int.h"
#include "util/io.h"
#include "util/json.h"
#include "util/string-view.h"
#include "util/string.h"
#include "util/string2.h"
#include "util/vector.h"

// TODO: Make symbolic representation of a pgn chess move.
// TODO: Do this in another file?
typedef String Move;

struct Puzzle {
    String id;
    U32 rating;
    Vector<Move> pgn;
    Vector<Move> solution;
};

static Puzzle
extractPuzzle(JsonDocument doc) noexcept {
    Puzzle p;

    p.id = doc.root["puzzle"]["id"].toString();
    p.rating = doc.root["puzzle"]["rating"].toInt();

    JsonValue pgn = doc.root["game"]["pgn"];
    Splits<' '> pastMoves(pgn.toString());
    StringView move = pastMoves.next();
    while (move.data) {
        p.pgn.push(move);
        move = pastMoves.next();
    }

    JsonValue solutionArray = doc.root["puzzle"]["solution"];
    for (JsonIterator it = begin(solutionArray);
         it != end(solutionArray);
         ++it) {
        p.solution.push(it->value.toString());
    }

    return p;
}

class LichessApi {
 public:
    Curl* curl;

 public:
    LichessApi() noexcept;
    ~LichessApi() noexcept;

    Puzzle
    getPuzzleDaily() noexcept;

    int
    postPuzzleComplete(StringView puzzle, bool win) noexcept;
};

LichessApi::LichessApi() noexcept {
    curl = curlMake();
}

LichessApi::~LichessApi() noexcept {
    curlDestroy(curl);
    curl = 0;
}

/*
{
  "game": {
    "id": "zS3sCvEA",
    "perf": {
      "icon": "_",
      "name": "Blitz"
    },
    "rated": true,
    "players": [
      {
        "userId": "yoanlabiblia7",
        "name": "yoanlabiblia7 (1849)",
        "color": "white"
      },
      {
        "userId": "lcheesecakel",
        "name": "lCheesecakel (1759)",
        "color": "black"
      }
    ],
    "pgn": "e4 c5 Nf3 Nc6 d4 cxd4 Nxd4 g6 Be3 Bg7 Nc3 Nf6 Ndb5 O-O f4 d6 Bd3 Ng4 Bg1 a6 Na3 Qa5 Qd2 Bd4 Bxd4 Nxd4 O-O Qb6 Nc4",
    "clock": "5+3"
  },
  "puzzle": {
    "id": "GI0Kh",
    "rating": 2273,
    "plays": 88823,
    "initialPly": 28,
    "solution": [
      "d4e2",
      "g1h1",
      "b6g1",
      "f1g1",
      "g4f2"
    ],
    "themes": [
      "middlegame",
      "long",
      "mateIn3",
      "sacrifice",
      "doubleCheck",
      "kingsideAttack",
      "smotheredMate"
    ]
  }
}
*/
Puzzle
LichessApi::getPuzzleDaily() noexcept {
    StringView url = "https://lichess.org/api/puzzle/daily";

    Vector<Header> headers;
    headers.push(Header("Host", "lichess.org"));
    headers.push(Header("Accept", "*/*"));

    String response;

    String err = curlGet(curl, &response, url, Vector<Header>());

    if (err.data) {
        serr << err << '\n';
        assert_(false);
    }

    sout << response << '\n';

    return extractPuzzle(JsonDocument(static_cast<String&&>(response)));
}

int
LichessApi::postPuzzleComplete(StringView puzzle, bool win) noexcept {
    String url = String() << "https://lichess.org/training/complete/mix/"
                          << puzzle;

    Vector<FormData> data;
    data.push(FormData("win", "false"));
    data.push(FormData("rated", "true"));

    String response;

    String err = curlForm(
        curl,
        &response,
        url,
        Vector<Header>(),
        static_cast<Vector<FormData>&&>(data)
    );

    if (err.data) {
        serr << err << '\n';
        return 1;
    }

    sout << response << '\n';

    return 0;
}

static void
rstrip(String& s) noexcept {
    while (s[s.size - 1] == '\n' || s[s.size - 1] == '\r') {
        s.size--;
    }
}

int
client() noexcept {
    String cookie;

    bool ok = readFile("cookie", cookie);
    if (!ok || !cookie.size) {
        serr << "Can't find file 'cookie'\n";
        return 1;
    }
    rstrip(cookie);

    LichessApi api;
    curlSetCookie(api.curl, cookie);

    Puzzle p = api.getPuzzleDaily();
    sout << "Puzzle id = " << p.id << '\n';
    sout << "Puzzle rating = " << p.rating << '\n';

    sout << "Puzzle pgn [" << '\n';
    for (String* move = p.pgn.begin();
         move != p.pgn.end();
         ++move) {
        sout << "  " << (*move) << '\n';
    }
    sout << "]\n";

    sout << "Puzzle solution [" << '\n';
    for (String* move = p.solution.begin();
         move != p.solution.end();
         ++move) {
        sout << "  " << (*move) << '\n';
    }
    sout << "]\n";

    //api.postPuzzleComplete("1pFNM", false);

    return 0;
}
