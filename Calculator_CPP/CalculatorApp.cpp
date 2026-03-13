#include "CalculatorApp.h"
#include <wx/gbsizer.h>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <algorithm>

#pragma warning(disable: 28251)

wxIMPLEMENT_APP(CalcApp);

// ---------------------------------------------------------------------------
// Colours
// ---------------------------------------------------------------------------
static const wxColour C_BLUE(70, 170, 230);
static const wxColour C_GREEN(145, 230, 50);
static const wxColour C_GRAY(170, 170, 170);
static const wxColour C_BLACK(0, 0, 0);
static const wxColour C_RED(200, 60, 60);
static const wxColour C_WHITE(255, 255, 255);
static const wxColour C_YELL(210, 200, 60);

// ---------------------------------------------------------------------------
// App
// ---------------------------------------------------------------------------
bool CalcApp::OnInit()
{
    (new CalcFrame())->Show();
    return true;
}

// ---------------------------------------------------------------------------
// MakeBtn
// ---------------------------------------------------------------------------
static wxButton* MakeBtn(wxPanel* p, int id, const wxString& lbl,
    const wxFont& f,
    const wxColour& fg, const wxColour& bg)
{
    wxButton* b = new wxButton(p, id, lbl,
        wxDefaultPosition, wxDefaultSize,
        wxBORDER_SIMPLE);
    b->SetFont(f);
    b->SetForegroundColour(fg);
    b->SetBackgroundColour(bg);
    return b;
}

// ---------------------------------------------------------------------------
// Frame constructor
// ---------------------------------------------------------------------------
CalcFrame::CalcFrame()
    : wxFrame(nullptr, wxID_ANY, "Calculator",
        wxDefaultPosition, wxSize(860, 660)),
    m_tok(0)
{
    SetMinSize(wxSize(700, 520));
    wxPanel* panel = new wxPanel(this);
    panel->SetBackgroundColour(C_BLACK);

    wxFont bigFont(15, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxFont normFont(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxFont resFont(22, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

    wxGridBagSizer* gbs = new wxGridBagSizer(3, 3);

    auto Add = [&](wxWindow* w, int r, int c, int rs = 1, int cs = 1) {
        gbs->Add(w, wxGBPosition(r, c), wxGBSpan(rs, cs),
            wxEXPAND | wxALL, 2);
        };

    Add(MakeBtn(panel, ID_BTN_HISTORY, "H", bigFont, C_BLACK, C_BLUE), 0, 0);
    wxPanel* blueGap = new wxPanel(panel);
    blueGap->SetBackgroundColour(C_BLUE);
    Add(blueGap, 1, 0);
    Add(MakeBtn(panel, ID_BTN_NEG, "-x", bigFont, C_BLACK, C_BLUE), 2, 0);
    Add(MakeBtn(panel, ID_BTN_FRAC, "X/X", bigFont, C_BLACK, C_BLUE), 3, 0, 2, 1);
    Add(MakeBtn(panel, ID_BTN_POW, "x^", bigFont, C_BLACK, C_BLUE), 5, 0);

    wxPanel* disp = new wxPanel(panel);
    disp->SetBackgroundColour(C_GREEN);
    wxBoxSizer* dv = new wxBoxSizer(wxVERTICAL);
    txtFormula = new wxTextCtrl(disp, wxID_ANY, "",
        wxDefaultPosition, wxDefaultSize,
        wxTE_RIGHT | wxTE_READONLY | wxBORDER_SIMPLE);
    txtFormula->SetFont(normFont);
    txtFormula->SetForegroundColour(C_BLACK);
    txtFormula->SetBackgroundColour(C_GREEN);
    txtFormula->SetHint("MOST RECENT FORMULA");
    txtInput = new wxTextCtrl(disp, wxID_ANY, "",
        wxDefaultPosition, wxDefaultSize,
        wxTE_RIGHT | wxBORDER_SIMPLE);
    txtInput->SetFont(resFont);
    txtInput->SetForegroundColour(C_BLACK);
    txtInput->SetBackgroundColour(C_GREEN);
    dv->Add(txtFormula, 1, wxEXPAND | wxALL, 4);
    dv->Add(txtInput, 2, wxEXPAND | wxALL, 4);
    disp->SetSizer(dv);
    Add(disp, 0, 1, 2, 4);

    Add(MakeBtn(panel, ID_BTN_CLEAR, "AC", bigFont, C_WHITE, C_RED), 0, 5);
    Add(MakeBtn(panel, ID_BTN_BACK, "<", bigFont, C_BLACK, C_GRAY), 1, 5);

    Add(MakeBtn(panel, ID_BTN_SIN_DEG, "Sin", normFont, C_BLACK, C_BLUE), 0, 6);
    Add(MakeBtn(panel, ID_BTN_SIN_RAD, "Sin()", normFont, C_BLACK, C_BLUE), 1, 6);
    Add(MakeBtn(panel, ID_BTN_COS_DEG, "Cos", normFont, C_BLACK, C_BLUE), 2, 6);
    Add(MakeBtn(panel, ID_BTN_COS_RAD, "Cos()", normFont, C_BLACK, C_BLUE), 3, 6);
    Add(MakeBtn(panel, ID_BTN_TAN_RAD, "Tan()", normFont, C_BLACK, C_BLUE), 4, 6);
    Add(MakeBtn(panel, ID_BTN_TAN_DEG, "Tan", normFont, C_BLACK, C_BLUE), 5, 6);

    struct BD { int id; const char* lbl; };
    const BD grid[4][5] = {
        {{ID_BTN_ADD,  "+"}, {ID_BTN_7,"7"},{ID_BTN_8,"8"},{ID_BTN_9,"9"},{ID_BTN_SQRT,"sqrt("}},
        {{ID_BTN_SUB,  "-"}, {ID_BTN_4,"4"},{ID_BTN_5,"5"},{ID_BTN_6,"6"},{ID_BTN_MUL, "x"}},
        {{ID_BTN_PCT,  "%"}, {ID_BTN_1,"1"},{ID_BTN_2,"2"},{ID_BTN_3,"3"},{ID_BTN_DIV, "/"}},
        {{ID_BTN_LPAREN,"("},{ID_BTN_DOT,"."},{ID_BTN_0,"0"},{ID_BTN_EQUALS,"="},{ID_BTN_RPAREN,")"}},
    };
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 5; ++c) {
            wxColour bg = (grid[r][c].id == ID_BTN_EQUALS) ? C_YELL : C_GRAY;
            Add(MakeBtn(panel, grid[r][c].id,
                wxString(grid[r][c].lbl), bigFont, C_BLACK, bg),
                r + 2, c + 1);
        }
    }

    for (int c = 0; c < 7; ++c) gbs->AddGrowableCol(c, 1);
    gbs->AddGrowableRow(0, 1);
    gbs->AddGrowableRow(1, 2);
    for (int r = 2; r < 6; ++r) gbs->AddGrowableRow(r, 1);

    Bind(wxEVT_BUTTON, &CalcFrame::OnButton, this, ID_BUTTON_FIRST, ID_BUTTON_LAST);

    panel->SetSizer(gbs);

    LoadHistory();
}

// ===========================================================================
// OnButton
// ===========================================================================
void CalcFrame::OnButton(wxCommandEvent& evt)
{
    const int id = evt.GetId();

    if (id == ID_BTN_CLEAR) {
        txtInput->Clear();
        m_resultShown = false;
        return;
    }
    if (id == ID_BTN_BACK) {
        wxString s = txtInput->GetValue();
        if (s.StartsWith("Error:")) {
            txtInput->Clear();
        }
        else if (!s.IsEmpty()) {
            static const char* multiFuncs[] = {
                "sqrt(", "sind(", "cosd(", "tand(",
                "sin(", "cos(", "tan(",
                "Sin ", "Cos ", "Tan ",
                nullptr
            };
            bool removed = false;
            for (int k = 0; multiFuncs[k]; ++k) {
                wxString fn(multiFuncs[k]);
                if (s.EndsWith(fn)) {
                    txtInput->SetValue(s.Left(s.Len() - fn.Len()));
                    removed = true;
                    break;
                }
            }
            if (!removed)
                txtInput->SetValue(s.RemoveLast());
        }
        return;
    }
    if (id == ID_BTN_HISTORY) {
        if (m_history.empty()) return;
        m_historyIdx = (m_historyIdx + 1) % (int)m_history.size();
        txtInput->SetValue(m_history[m_history.size() - 1 - m_historyIdx]);
        return;
    }
    if (id == ID_BTN_EQUALS) {
        wxString expr = txtInput->GetValue().Trim(true).Trim(false);
        if (expr.IsEmpty()) return;
        try {
            double result = Evaluate(expr);
            m_lastExpr = expr;
            txtFormula->SetValue(expr);
            m_history.push_back(expr);
            wxString res;
            if (result == std::floor(result) && std::abs(result) < 1e15)
                res = wxString::Format("%.0f", result);
            else
                res = wxString::Format("%.10g", result);
            txtInput->SetValue(res);
            m_resultShown = true;
            SaveHistory();
        }
        catch (const std::exception& ex) {
            txtInput->SetValue(wxString("Error: ") + ex.what());
        }
        catch (...) {
            txtInput->SetValue("Error: unknown error");
        }
        return;
    }

    if (m_resultShown) {
        bool isNumber = (id >= ID_BTN_0 && id <= ID_BTN_9) || id == ID_BTN_DOT;
        if (isNumber)
            txtInput->Clear();
        m_resultShown = false;
    }

    struct AppendEntry { int id; const char* str; };
    static const AppendEntry tbl[] = {
        {ID_BTN_0,"0"}, {ID_BTN_1,"1"}, {ID_BTN_2,"2"},
        {ID_BTN_3,"3"}, {ID_BTN_4,"4"}, {ID_BTN_5,"5"},
        {ID_BTN_6,"6"}, {ID_BTN_7,"7"}, {ID_BTN_8,"8"}, {ID_BTN_9,"9"},
        {ID_BTN_ADD,"+"}, {ID_BTN_SUB,"-"}, {ID_BTN_MUL,"*"},
        {ID_BTN_DIV,"/"}, {ID_BTN_PCT,"%"}, {ID_BTN_FRAC,"/"},
        {ID_BTN_DOT,"."}, {ID_BTN_LPAREN,"("}, {ID_BTN_RPAREN,")"},
        {ID_BTN_NEG,"-"}, {ID_BTN_SQRT,"sqrt("}, {ID_BTN_POW,"^"},
        {ID_BTN_SIN_RAD,"sin("}, {ID_BTN_COS_RAD,"cos("}, {ID_BTN_TAN_RAD,"tan("},
        {ID_BTN_SIN_DEG,"Sin "}, {ID_BTN_COS_DEG,"Cos "}, {ID_BTN_TAN_DEG,"Tan "},
    };

    for (const auto& e : tbl) {
        if (e.id == id) {
            txtInput->AppendText(e.str);
            return;
        }
    }
}

// ===========================================================================
// History persistence
// ===========================================================================
static wxString HistoryFilePath()
{
    wxFileName f(wxStandardPaths::Get().GetUserDataDir(), "history.txt");
    wxFileName::Mkdir(f.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    return f.GetFullPath();
}

void CalcFrame::SaveHistory()
{
    std::ofstream out(HistoryFilePath().ToStdString(), std::ios::trunc);
    if (!out) return;
    out << m_lastExpr.ToStdString() << "\n";
    for (const wxString& entry : m_history)
        out << entry.ToStdString() << "\n";
}

void CalcFrame::LoadHistory()
{
    std::ifstream in(HistoryFilePath().ToStdString());
    if (!in) return;
    std::string line;
    bool first = true;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        wxString entry(line);
        if (first) {
            m_lastExpr = entry;
            txtFormula->ChangeValue(entry);
            first = false;
        }
        m_history.push_back(entry);
    }
}

// ===========================================================================
// Lexer
// ===========================================================================
static bool isnamechar(char c) {
    return std::isalpha((unsigned char)c);
}

void CalcFrame::Lex(const std::string& src)
{
    m_tokens.clear();
    size_t i = 0;

    while (i < src.size()) {
        char c = src[i];

        if (std::isspace((unsigned char)c)) { ++i; continue; }

        if (std::isdigit((unsigned char)c) || c == '.') {
            size_t start = i;
            int dots = 0;
            while (i < src.size()) {
                char nc = src[i];
                bool expSign = ((nc == '+' || nc == '-') && i > 0 &&
                    (src[i - 1] == 'e' || src[i - 1] == 'E'));
                if (!std::isdigit((unsigned char)nc) &&
                    nc != '.' && nc != 'e' && nc != 'E' && !expSign)
                    break;
                if (nc == '.') ++dots;
                ++i;
            }
            if (dots > 1)
                throw std::runtime_error("invalid number: too many decimal points");
            std::string numStr = src.substr(start, i - start);
            if (numStr == ".")
                throw std::runtime_error("invalid number: lone decimal point");
            Token t;
            t.type = TokType::NUMBER;
            try {
                t.num = std::stod(numStr);
            }
            catch (const std::invalid_argument&) {
                throw std::runtime_error("invalid number: " + numStr);
            }
            catch (const std::out_of_range&) {
                throw std::runtime_error("number out of range: " + numStr);
            }
            m_tokens.push_back(t);
            continue;
        }

        if (isnamechar(c)) {
            size_t start = i;
            while (i < src.size() && isnamechar(src[i])) ++i;
            std::string word = src.substr(start, i - start);
            std::string low = word;
            std::transform(low.begin(), low.end(), low.begin(),
                [](unsigned char ch) { return std::tolower(ch); });
            static const char* known[] = {
                "sin","cos","tan","sind","cosd","tand","sqrt", nullptr
            };
            bool ok = false;
            for (int k = 0; known[k]; ++k) if (low == known[k]) { ok = true; break; }
            if (!ok)
                throw std::runtime_error("unknown name: " + word);
            Token t;
            t.type = TokType::FUNC;
            t.func = low;
            m_tokens.push_back(t);
            continue;
        }

        Token t;
        switch (c) {
        case '+': t.type = TokType::PLUS;    break;
        case '-': t.type = TokType::MINUS;   break;
        case '*': t.type = TokType::STAR;    break;
        case '/': t.type = TokType::SLASH;   break;
        case '%': t.type = TokType::PERCENT; break;
        case '^': t.type = TokType::CARET;   break;
        case '(': t.type = TokType::LPAREN;  break;
        case ')': t.type = TokType::RPAREN;  break;
        default:
            throw std::runtime_error(
                std::string("unexpected character '") + c + "'");
        }
        m_tokens.push_back(t);
        ++i;
    }

    Token end; end.type = TokType::END;
    m_tokens.push_back(end);
}

Token& CalcFrame::Peek() { return m_tokens[m_tok]; }

Token CalcFrame::Consume()
{
    Token t = m_tokens[m_tok];
    if (t.type != TokType::END) ++m_tok;
    return t;
}

// ===========================================================================
// Parser
// ===========================================================================
double CalcFrame::ParsePrimary()
{
    Token t = Peek();

    if (t.type == TokType::LPAREN) {
        Consume();
        double val = ParseExpr();
        if (Peek().type != TokType::RPAREN)
            throw std::runtime_error("missing ')'");
        Consume();
        return val;
    }

    if (t.type == TokType::NUMBER) {
        Consume();
        return t.num;
    }

    if (t.type == TokType::FUNC) {
        Consume();
        const std::string fn = t.func;

        double arg;
        if (Peek().type == TokType::LPAREN) {
            Consume();
            arg = ParseExpr();
            if (Peek().type != TokType::RPAREN)
                throw std::runtime_error("missing ')' in " + fn);
            Consume();
        }
        else {
            arg = ParseUnary();
        }

        const double PI = 3.14159265358979323846;
        if (fn == "sin")  return std::sin(arg);
        if (fn == "cos")  return std::cos(arg);
        if (fn == "tan")  return std::tan(arg);
        if (fn == "sind") return std::sin(arg * PI / 180.0);
        if (fn == "cosd") return std::cos(arg * PI / 180.0);
        if (fn == "tand") return std::tan(arg * PI / 180.0);
        if (fn == "sqrt") {
            if (arg < 0.0) throw std::runtime_error("sqrt of negative");
            return std::sqrt(arg);
        }
        throw std::runtime_error("unknown function: " + fn);
    }

    throw std::runtime_error("expected a value");
}

double CalcFrame::ParseUnary()
{
    if (Peek().type == TokType::MINUS) { Consume(); return -ParseUnary(); }
    if (Peek().type == TokType::PLUS) { Consume(); return +ParseUnary(); }
    return ParsePrimary();
}

double CalcFrame::ParsePower()
{
    double base = ParseUnary();
    if (Peek().type == TokType::CARET) {
        Consume();
        return std::pow(base, ParsePower());
    }
    return base;
}

double CalcFrame::ParseTerm()
{
    double val = ParsePower();
    while (true) {
        TokType tt = Peek().type;
        if (tt != TokType::STAR && tt != TokType::SLASH && tt != TokType::PERCENT)
            break;
        Consume();
        double rhs = ParsePower();
        if (tt == TokType::STAR)       val *= rhs;
        else if (tt == TokType::SLASH) {
            if (rhs == 0.0) throw std::runtime_error("division by zero");
            val /= rhs;
        }
        else {
            if (rhs == 0.0) throw std::runtime_error("modulo by zero");
            val = std::fmod(val, rhs);
        }
    }
    return val;
}

double CalcFrame::ParseExpr()
{
    double val = ParseTerm();
    while (true) {
        TokType tt = Peek().type;
        if (tt != TokType::PLUS && tt != TokType::MINUS) break;
        Consume();
        double rhs = ParseTerm();
        val = (tt == TokType::PLUS) ? val + rhs : val - rhs;
    }
    return val;
}

double CalcFrame::Evaluate(const wxString& expr)
{
    Lex(expr.ToStdString());

    for (size_t i = 0; i + 1 < m_tokens.size(); ++i) {
        auto isOp = [](TokType t) {
            return t == TokType::PLUS || t == TokType::STAR ||
                t == TokType::SLASH || t == TokType::PERCENT ||
                t == TokType::CARET;
            };
        if (isOp(m_tokens[i].type) && isOp(m_tokens[i + 1].type))
            throw std::runtime_error("invalid expression: consecutive operators");
        if (m_tokens[i].type == TokType::END)
            break;
    }

    m_tok = 0;
    double result = ParseExpr();
    if (Peek().type != TokType::END)
        throw std::runtime_error("unexpected token");
    return result;
}