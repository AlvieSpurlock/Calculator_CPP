#pragma once
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <vector>
#include <string>
#include <fstream>

// ---------------------------------------------------------------------------
// Button IDs
// ---------------------------------------------------------------------------
enum {
    ID_BUTTON_FIRST = 1000,
    ID_BTN_0 = 1000, ID_BTN_1, ID_BTN_2, ID_BTN_3, ID_BTN_4,
    ID_BTN_5, ID_BTN_6, ID_BTN_7, ID_BTN_8, ID_BTN_9,
    ID_BTN_ADD, ID_BTN_SUB, ID_BTN_MUL, ID_BTN_DIV, ID_BTN_PCT,
    ID_BTN_DOT, ID_BTN_LPAREN, ID_BTN_RPAREN,
    ID_BTN_NEG, ID_BTN_SQRT, ID_BTN_POW, ID_BTN_FRAC,
    ID_BTN_SIN_RAD, ID_BTN_COS_RAD, ID_BTN_TAN_RAD,
    ID_BTN_SIN_DEG, ID_BTN_COS_DEG, ID_BTN_TAN_DEG,
    ID_BTN_CLEAR, ID_BTN_BACK, ID_BTN_EQUALS, ID_BTN_HISTORY,
    ID_BUTTON_LAST = 2000
};

// ---------------------------------------------------------------------------
// Token types
// ---------------------------------------------------------------------------
enum class TokType {
    NUMBER,
    PLUS, MINUS, STAR, SLASH, PERCENT, CARET,
    FUNC,
    LPAREN, RPAREN,
    END
};

struct Token {
    TokType     type = TokType::END;
    double      num = 0.0;
    std::string func;
};

// ---------------------------------------------------------------------------
// CalcFrame
// ---------------------------------------------------------------------------
class CalcFrame : public wxFrame {
public:
    CalcFrame();

private:
    wxTextCtrl* txtFormula;
    wxTextCtrl* txtInput;
    wxString              m_lastExpr;
    bool                  m_resultShown = false;
    int                   m_historyIdx = -1;
    std::vector<wxString> m_history;

    void OnButton(wxCommandEvent& evt);
    void SaveHistory();
    void LoadHistory();
    double Evaluate(const wxString& expr);

    std::vector<Token> m_tokens;
    size_t             m_tok;

    void   Lex(const std::string& src);
    Token& Peek();
    Token  Consume();

    double ParseExpr();
    double ParseTerm();
    double ParsePower();
    double ParseUnary();
    double ParsePrimary();

};

// ---------------------------------------------------------------------------
// CalcApp
// ---------------------------------------------------------------------------
class CalcApp : public wxApp {
public:
    bool OnInit() override;
};
wxDECLARE_APP(CalcApp);