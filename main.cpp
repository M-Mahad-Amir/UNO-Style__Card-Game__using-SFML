#include <SFML/Graphics.hpp>
// #include <SFML/Audio.hpp>
#include "UNO_Game.h"
#include <algorithm>
#include <string>
#include <vector>

/* SFML based GUI --- Frontend related components, renderings etc. */

const unsigned WIN_W = 1366;
const unsigned WIN_H = 820;

enum class Screen
{
    MENU,
    RULES,
    SETUP,
    GAME
};

struct HandLayout
{
    float startX;
    float cardY;
    float cw;
    float gap;
};

HandLayout computeHandLayout(int handSz)
{
    float cw = 118.f;
    float gap = 8.f;
    float totalW = handSz * (cw + gap) - gap;
    float availW = float(WIN_W) - 100.f;

    if (handSz > 0 && totalW > availW)
    {
        cw = (availW - gap * (handSz - 1)) / handSz;
        gap = (handSz > 1) ? (availW - cw * handSz) / (handSz - 1) : 0.f;
        totalW = handSz * (cw + gap) - gap;
    }

    HandLayout layout;
    layout.startX = std::max(50.f, (float(WIN_W) - totalW) / 2.f);
    layout.cardY = float(WIN_H) - 200.f;
    layout.cw = cw;
    layout.gap = gap;
    return layout;
}

const sf::FloatRect DISCARD_PILE_RECT({478.f, 175.f}, {170.f, 220.f});
const sf::FloatRect DRAW_PILE_RECT({892.f, 175.f}, {170.f, 220.f});
const sf::FloatRect BTN_DRAW_CARD_RECT({890.f, 410.f}, {170.f, 40.f});
const sf::FloatRect BTN_PLAY_IT_RECT({360.f, 510.f}, {180.f, 48.f});
const sf::FloatRect BTN_KEEP_IT_RECT({570.f, 510.f}, {180.f, 48.f});
const sf::FloatRect UNO_BUTTON_RECT({1090.f, 60.f}, {220.f, 62.f}); 
sf::Color toSFColor(Color c)
{
    switch (c)
    {
    case Color::RED:    return sf::Color(210, 45, 45);
    case Color::BLUE:   return sf::Color(40, 100, 210);
    case Color::GREEN:  return sf::Color(40, 170, 70);
    case Color::YELLOW: return sf::Color(220, 190, 20);
    default:            return sf::Color(55, 55, 55);
    }
}

void drawUnoCard(sf::RenderWindow &win, sf::Font &font,
                 const Card *card,
                 float x, float y,
                 float w = 108.f, float h = 145.f,
                 bool highlight = false)
{
    sf::RectangleShape rect({w, h});
    rect.setPosition({x, y});
    rect.setFillColor(toSFColor(card->getColor()));
    rect.setOutlineColor(highlight ? sf::Color::Yellow : sf::Color::White);
    rect.setOutlineThickness(highlight ? 3.5f : 1.5f);
    win.draw(rect);

    sf::Text lbl(font, card->getLabel(), 18);
    lbl.setFillColor(sf::Color::White);
    sf::FloatRect lb = lbl.getLocalBounds();
    lbl.setOrigin({lb.size.x / 2.f, lb.size.y / 2.f});
    lbl.setPosition({x + w / 2.f, y + h / 2.f});
    win.draw(lbl);
}

sf::FloatRect drawButton(sf::RenderWindow &win, sf::Font &font,
                         const std::string &text,
                         float x, float y, float w, float h,
                         sf::Color fill, unsigned charSize = 20)
{
    sf::RectangleShape box({w, h});
    box.setPosition({x, y});
    box.setFillColor(fill);
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(2.f);
    win.draw(box);

    sf::Text lbl(font, text, charSize);
    lbl.setFillColor(sf::Color::White);
    sf::FloatRect lb = lbl.getLocalBounds();
    lbl.setOrigin({lb.size.x / 2.f, lb.size.y / 2.f});
    lbl.setPosition({x + w / 2.f, y + h / 2.f});
    win.draw(lbl);

    return sf::FloatRect({x, y}, {w, h});
}

sf::FloatRect drawPillButton(sf::RenderWindow &win, sf::Font &font,
                             const std::string &text,
                             float x, float y, float w, float h,
                             sf::Color fill, unsigned charSize = 20)
{
    float radius = h / 2.f;

    // Left cap
    sf::CircleShape leftCap(radius);
    leftCap.setPosition({x, y});
    leftCap.setFillColor(fill);
    leftCap.setOutlineColor(sf::Color::White);
    leftCap.setOutlineThickness(2.f);
    win.draw(leftCap);

    // Right cap
    sf::CircleShape rightCap(radius);
    rightCap.setPosition({x + w - 2.f * radius, y});
    rightCap.setFillColor(fill);
    rightCap.setOutlineColor(sf::Color::White);
    rightCap.setOutlineThickness(2.f);
    win.draw(rightCap);

    // Centre rectangle — no outline, overlaps caps to hide seams
    sf::RectangleShape mid({w - 2.f * radius, h});
    mid.setPosition({x + radius, y});
    mid.setFillColor(fill);
    win.draw(mid);

    // Label centred on full pill bounding box
    sf::Text lbl(font, text, charSize);
    lbl.setFillColor(sf::Color::White);
    lbl.setStyle(sf::Text::Bold);
    sf::FloatRect lb = lbl.getLocalBounds();
    lbl.setOrigin({lb.size.x / 2.f, lb.size.y / 2.f});
    lbl.setPosition({x + w / 2.f, y + h / 2.f});
    win.draw(lbl);

    return sf::FloatRect({x, y}, {w, h});
}

void drawMenuScreen(sf::RenderWindow &window, sf::Font &font)
{
    sf::RectangleShape panel({600.f, 380.f});
    panel.setPosition({(WIN_W - 600.f) / 2.f, 150.f});
    panel.setFillColor(sf::Color(22, 52, 22));
    panel.setOutlineColor(sf::Color(110, 110, 210));
    panel.setOutlineThickness(3.f);
    window.draw(panel);

    sf::Text heading(font, "CLASSIC UNO", 48);
    heading.setFillColor(sf::Color::Red);
    heading.setStyle(sf::Text::Bold);
    sf::FloatRect hb = heading.getLocalBounds();
    heading.setOrigin({hb.size.x / 2.f, 0.f});
    heading.setPosition({float(WIN_W) / 2.f, 178.f});
    window.draw(heading);

    sf::Text tag(font, "Jump right in!", 24);
    tag.setFillColor(sf::Color(160, 160, 220));
    sf::FloatRect tb = tag.getLocalBounds();
    tag.setOrigin({tb.size.x / 2.f, 0.f});
    tag.setPosition({float(WIN_W) / 2.f, 228.f});
    window.draw(tag);

    float bx = (WIN_W - 340.f) / 2.f;
    drawButton(window, font, "Setup Game", bx, 290, 340, 56, sf::Color(60, 60, 140), 22);
    drawButton(window, font, "Rules", bx, 368, 340, 56, sf::Color(40, 100, 100), 22);
    drawButton(window, font, "Quit Game", bx, 446, 340, 56, sf::Color(130, 40, 40), 22);
}

void drawRulesScreen(sf::RenderWindow &window, sf::Font &font)
{
    sf::RectangleShape panel({float(WIN_W) - 100.f, float(WIN_H) - 80.f});
    panel.setPosition({40.f, 30.f});
    panel.setFillColor(sf::Color(22, 22, 58));
    panel.setOutlineColor(sf::Color(110, 110, 210));
    panel.setOutlineThickness(3.f);
    window.draw(panel);

    sf::Text title(font, "UNO  RULES", 50);
    title.setFillColor(sf::Color(200, 200, 255));
    title.setStyle(sf::Text::Bold);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin({tb.size.x / 2.f, 0.f});
    title.setPosition({float(WIN_W) / 2.f, 90.f});
    window.draw(title);

    const char *rules[] = {
    "1.  Each player starts with 7 cards.",
    "2.  Match the top card by COLOR, NUMBER, or SYMBOL.",
    "3.  SKIP:             The next player loses their turn.",
    "4.  REVERSE:    2-player: acts as Skip.  3-4 players: reverses turn order.",
    "5.  DRAW TWO: Next player draws 2 cards but still takes their turn.",
    "6.  WILD:             Choose any color. If you have no cards of that color,",
    "                          your turn ends immediately.",
    "7.  WILD DRAW 4: Next player draws 4 cards but still takes their turn.",
    "                          Then choose a color (same Wild logic applies).",
    "8.  If you cannot play, draw one card. If it is playable, you may play it.",
    "9.  UNO:              Press the UNO button when you have 1 card left!",
    "                          Failing to press it before your next action costs you 6 cards.",
    "10. First player to empty their hand WINS!"};

    float ry = 190.f;
    for (auto &r : rules)
    {
        sf::Text rt(font, r, 24);
        rt.setFillColor(sf::Color(215, 215, 215));
        rt.setPosition({180.f, ry});
        window.draw(rt);
        ry += 36.f;
    }

    drawButton(window, font, "Back", 583, 754, 200, 46, sf::Color(70, 70, 140));
}

void drawSetupScreen(sf::RenderWindow &window, sf::Font &font,
                     bool setupCountChosen,
                     int setupPlayerCount,
                     int activeNameField,
                     const std::vector<std::string> &setupNames)
{
    sf::RectangleShape panel({float(WIN_W) - 100.f, float(WIN_H) - 80.f});
    panel.setPosition({40.f, 30.f});
    panel.setFillColor(sf::Color(42, 26, 20));
    panel.setOutlineColor(sf::Color(240, 200, 120));
    panel.setOutlineThickness(3.f);
    window.draw(panel);

    sf::Text title(font, "GAME SETUP", 50);
    title.setFillColor(sf::Color(255, 230, 170));
    title.setStyle(sf::Text::Bold);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin({tb.size.x / 2.f, 0.f});
    title.setPosition({float(WIN_W) / 2.f, 85.f});
    window.draw(title);

    sf::Text hint(font, "Choose 2, 3, or 4 players, then enter names. Use Tab to switch between Player Names", 22);
    hint.setFillColor(sf::Color(235, 235, 235));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin({hb.size.x / 2.f, 0.f});
    hint.setPosition({float(WIN_W) / 2.f, 145.f});
    window.draw(hint);

    const float countW = 120.f;
    const float countH = 60.f;
    const float countGap = 25.f;
    float countX = (float(WIN_W) - (3.f * countW + 2.f * countGap)) / 2.f;
    float countY = 205.f;

    for (int i = 0; i < 3; i++)
    {
        bool active = setupCountChosen && setupPlayerCount == 2 + i;
        sf::RectangleShape box({countW, countH});
        box.setPosition({countX + i * (countW + countGap), countY});
        box.setFillColor(active ? sf::Color(130, 85, 20) : sf::Color(70, 50, 35));
        box.setOutlineColor(active ? sf::Color::Yellow : sf::Color(240, 200, 120));
        box.setOutlineThickness(2.f);
        window.draw(box);

        sf::Text n(font, std::to_string(2 + i), 32);
        n.setFillColor(sf::Color::White);
        n.setStyle(sf::Text::Bold);
        sf::FloatRect nb = n.getLocalBounds();
        n.setOrigin({nb.size.x / 2.f, nb.size.y / 2.f});
        n.setPosition({countX + i * (countW + countGap) + countW / 2.f, countY + countH / 2.f});
        window.draw(n);
    }

    if (setupCountChosen)
    {
        const float nameX = 370.f;
        const float nameY = 320.f;
        const float nameW = 626.f;
        const float nameH = 44.f;
        const float nameGap = 14.f;

        for (int i = 0; i < setupPlayerCount; i++)
        {
            bool active = (i == activeNameField);
            sf::RectangleShape field({nameW, nameH});
            field.setPosition({nameX, nameY + i * (nameH + nameGap)});
            field.setFillColor(active ? sf::Color(70, 42, 26) : sf::Color(50, 34, 24));
            field.setOutlineColor(active ? sf::Color(255, 230, 170) : sf::Color(200, 170, 120));
            field.setOutlineThickness(2.f);
            window.draw(field);

            std::string value = setupNames[i];
            if (value.empty()) value = "Type player name...";
            sf::Text t(font, value, 22);
            t.setFillColor(setupNames[i].empty() ? sf::Color(170, 170, 170) : sf::Color::White);
            t.setPosition({nameX + 14.f, nameY + i * (nameH + nameGap) + 7.f});
            window.draw(t);
        }

        drawButton(window, font, "Start Game", (WIN_W - 260.f) / 2.f, 670.f, 260.f, 52.f, sf::Color(90, 120, 50), 22);
    }
}

void drawPlayerTags(sf::RenderWindow &window, sf::Font &font, GameManager *gm)
{
    int playerCount = gm->getPlayerCount();
    int currentIdx = gm->getCurrentPlayerIdx();
    const float tagX = 40.f;
    const float tagY = 150.f;
    const float tagW = 210.f;
    const float tagH = 65.f;
    const float tagGap = 14.f;

    for (int i = 0; i < playerCount; i++)
    {
        bool active = (i == currentIdx);
        sf::RectangleShape tag({tagW, tagH});
        tag.setPosition({tagX, tagY + i * (tagH + tagGap)});
        tag.setFillColor(active ? sf::Color(85, 40, 135) : sf::Color(35, 35, 75));
        tag.setOutlineColor(active ? sf::Color(245, 200, 255) : sf::Color(75, 75, 120));
        tag.setOutlineThickness(active ? 3.f : 1.5f);
        window.draw(tag);

        Player *player = gm->getPlayer(i);
        if (!player) continue;

        sf::Text name(font, player->getName(), 20);
        name.setFillColor(active ? sf::Color(245, 220, 255) : sf::Color(175, 175, 175));
        if (active) name.setStyle(sf::Text::Bold);
        sf::FloatRect nb = name.getLocalBounds();
        name.setOrigin({nb.size.x / 2.f, nb.size.y / 2.f});
        name.setPosition({tagX + tagW / 2.f, tagY + i * (tagH + tagGap) + 27.f});
        window.draw(name);

        sf::Text cards(font, "Cards: " + std::to_string(player->getHandSize()), 15);
        cards.setFillColor(sf::Color(190, 190, 190));
        cards.setPosition({tagX + 13.f, tagY + i * (tagH + tagGap) + 43.f});
        window.draw(cards);
    }
}

void drawGameScreen(sf::RenderWindow &window, sf::Font &font, GameManager *gm,
                    bool &showMsg, sf::Clock &msgClock,
                    bool &unoAcknowledged, int &unoOwnerIdx)
{
    TurnPhase phase = gm->getTurnPhase();
    Player *cur = gm->getCurrentPlayer();
    int currentIdx = gm->getCurrentPlayerIdx();

    if (phase == TurnPhase::CHOOSE_ACTION && cur && cur->getHandSize() == 1)
    {
        if (unoOwnerIdx != currentIdx)
        {
            unoOwnerIdx = currentIdx;
            unoAcknowledged = false;
        }
    }
    else if (phase == TurnPhase::GAME_OVER || !cur || cur->getHandSize() != 1)
    {
        unoOwnerIdx = -1;
        unoAcknowledged = false;
    }

    if (phase == TurnPhase::GAME_OVER)
    {
        sf::RectangleShape tint({float(WIN_W), float(WIN_H)});
        tint.setFillColor(sf::Color(0, 0, 0, 185));
        window.draw(tint);

        const float panelW = 480.f, panelH = 280.f;
        const float panelX = (float(WIN_W) - panelW) / 2.f;
        const float panelY = (float(WIN_H) - panelH) / 2.f;
        const float cx = float(WIN_W) / 2.f;

        sf::RectangleShape panel({panelW, panelH});
        panel.setPosition({panelX, panelY});
        panel.setFillColor(sf::Color(22, 55, 22));
        panel.setOutlineColor(sf::Color::Yellow);
        panel.setOutlineThickness(4.f);
        window.draw(panel);

        std::string winMsg = gm->getWinner()->getName() + "  WINS!";
        sf::Text wt(font, winMsg, 44);
        wt.setFillColor(sf::Color::Yellow);
        wt.setStyle(sf::Text::Bold);
        sf::FloatRect wb = wt.getLocalBounds();
        wt.setOrigin({wb.size.x / 2.f, wb.size.y / 2.f});
        wt.setPosition({cx, panelY + 55.f});
        window.draw(wt);

        sf::Text sub(font, "Congratulations!", 22);
        sub.setFillColor(sf::Color::White);
        sf::FloatRect sb = sub.getLocalBounds();
        sub.setOrigin({sb.size.x / 2.f, sb.size.y / 2.f});
        sub.setPosition({cx, panelY + 125.f});
        window.draw(sub);

        sf::Text hint(font, "Press  'New Game'  to play again.", 16);
        hint.setFillColor(sf::Color(180, 180, 180));
        sf::FloatRect hb = hint.getLocalBounds();
        hint.setOrigin({hb.size.x / 2.f, hb.size.y / 2.f});
        hint.setPosition({cx, panelY + 165.f});
        window.draw(hint);

        drawButton(window, font, "New Game", cx - 160.f, panelY + 230.f, 340.f, 52.f, sf::Color(45, 110, 45));
        drawButton(window, font, "Quit Game", cx - 160.f, panelY + 285.f, 340.f, 52.f, sf::Color(110, 45, 45));
        // for 
        return;
    }

    sf::RectangleShape felt({float(WIN_W) - 80.f, float(WIN_H) - 60.f});
    felt.setPosition({40.f, 30.f});
    felt.setFillColor(sf::Color(128, 0, 0));
    felt.setOutlineColor(sf::Color::Yellow);
    felt.setOutlineThickness(3.f);
    window.draw(felt);

    sf::Text title(font, "GAME IN SESSION", 24);
    title.setFillColor(sf::Color(170, 255, 170));
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleB = title.getLocalBounds();
    title.setOrigin({titleB.size.x / 2.f, 0.f});
    title.setPosition({float(WIN_W) / 2.f, 40.f});
    window.draw(title);

    sf::Text turnTxt(font, cur->getName() + "'s  Turn", 17);
    turnTxt.setFillColor(sf::Color(255, 215, 80));
    sf::FloatRect ttb = turnTxt.getLocalBounds();
    turnTxt.setOrigin({ttb.size.x / 2.f, 0.f});
    turnTxt.setPosition({float(WIN_W) / 2.f, 72.f});
    window.draw(turnTxt);

    drawPlayerTags(window, font, gm);

    sf::Text discardLbl(font, "DISCARD PILE", 20);
    discardLbl.setFillColor(sf::Color(190, 255, 190));
    sf::FloatRect dlb = discardLbl.getLocalBounds();
    discardLbl.setOrigin({dlb.size.x / 2.f, 0.f});
    discardLbl.setPosition({DISCARD_PILE_RECT.position.x + DISCARD_PILE_RECT.size.x / 3.f, 150.f});
    window.draw(discardLbl);

    sf::Text drawLbl(font, "DRAW PILE", 20);
    drawLbl.setFillColor(sf::Color(190, 255, 190));
    sf::FloatRect dwb = drawLbl.getLocalBounds();
    drawLbl.setOrigin({dwb.size.x / 2.f, 0.f});
    drawLbl.setPosition({DRAW_PILE_RECT.position.x + DRAW_PILE_RECT.size.x / 2.f, 150.f});
    window.draw(drawLbl);

    if (gm->getTopCard())
        drawUnoCard(window, font, gm->getTopCard(), 460.f, 175.f, 170.f, 220.f);

    sf::RectangleShape pile(DRAW_PILE_RECT.size);
    pile.setPosition(DRAW_PILE_RECT.position);
    pile.setFillColor(sf::Color(33, 33, 33));
    pile.setOutlineColor(sf::Color::White);
    pile.setOutlineThickness(3.f);
    window.draw(pile);

    sf::Text pileLabel(font, "UNO", 28);
    pileLabel.setFillColor(sf::Color::White);
    pileLabel.setStyle(sf::Text::Bold);
    sf::FloatRect plb = pileLabel.getLocalBounds();
    pileLabel.setOrigin({plb.size.x / 2.f, plb.size.y / 2.f});
    pileLabel.setPosition({DRAW_PILE_RECT.position.x + DRAW_PILE_RECT.size.x / 2.f,
                           DRAW_PILE_RECT.position.y + DRAW_PILE_RECT.size.y / 2.f});
    window.draw(pileLabel);

    sf::RectangleShape btn({BTN_DRAW_CARD_RECT.size.x, BTN_DRAW_CARD_RECT.size.y});
    btn.setPosition(BTN_DRAW_CARD_RECT.position);
    btn.setFillColor(sf::Color(120, 52, 20));
    btn.setOutlineColor(sf::Color(235, 170, 90));
    btn.setOutlineThickness(2.f);
    window.draw(btn);

    sf::Text dt(font, "DRAW CARD", 22);
    dt.setFillColor(sf::Color::White);
    dt.setStyle(sf::Text::Bold);
    sf::FloatRect dtb = dt.getLocalBounds();
    dt.setOrigin({dtb.size.x / 2.f, dtb.size.y / 2.f});
    dt.setPosition({BTN_DRAW_CARD_RECT.position.x + BTN_DRAW_CARD_RECT.size.x / 2.f,
                    BTN_DRAW_CARD_RECT.position.y + BTN_DRAW_CARD_RECT.size.y / 2.f});
    window.draw(dt);

    const auto &hand = cur->getHand();
    int sz = (int)hand.size();
    HandLayout layout = computeHandLayout(sz);

    sf::Text handLbl(font, cur->getName() + "'s  Hand:", 17);
    handLbl.setFillColor(sf::Color(170, 255, 170));
    handLbl.setPosition({layout.startX, float(WIN_H) - 222.f});
    window.draw(handLbl);

    for (int i = 0; i < sz; i++)
    {
        bool hi = (hand[i] == gm->getLastDrawnCard());
        drawUnoCard(window, font, hand[i],
                    layout.startX + i * (layout.cw + layout.gap), layout.cardY,
                    layout.cw, 160.f, hi);
    }

    if (phase == TurnPhase::AWAITING_DRAW_DECISION)
    {
        sf::RectangleShape ov({440.f, 90.f});
        ov.setPosition({320.f, 470.f});
        ov.setFillColor(sf::Color(12, 12, 52, 238));
        ov.setOutlineColor(sf::Color::White);
        ov.setOutlineThickness(1.5f);
        window.draw(ov);

        sf::Text msg(font, "You drew a playable card!", 18);
        msg.setFillColor(sf::Color::White);
        sf::FloatRect mb = msg.getLocalBounds();
        msg.setOrigin({mb.size.x / 2.f, 0.f});
        msg.setPosition({540.f, 477.f});
        window.draw(msg);

        drawButton(window, font, "Play It",
                   BTN_PLAY_IT_RECT.position.x, BTN_PLAY_IT_RECT.position.y,
                   BTN_PLAY_IT_RECT.size.x, BTN_PLAY_IT_RECT.size.y,
                   sf::Color(35, 100, 35), 20);
        drawButton(window, font, "Keep It",
                   BTN_KEEP_IT_RECT.position.x, BTN_KEEP_IT_RECT.position.y,
                   BTN_KEEP_IT_RECT.size.x, BTN_KEEP_IT_RECT.size.y,
                   sf::Color(110, 60, 15), 20);
    }

    if (phase == TurnPhase::CHOOSE_COLOR)
    {
        sf::RectangleShape ovBg({530.f, 210.f});
        ovBg.setPosition({310.f, 230.f});
        ovBg.setFillColor(sf::Color(8, 8, 38, 242));
        ovBg.setOutlineColor(sf::Color::White);
        ovBg.setOutlineThickness(2.f);
        window.draw(ovBg);

        sf::Text pick(font, "Choose a Color:", 24);
        pick.setFillColor(sf::Color::White);
        sf::FloatRect pb = pick.getLocalBounds();
        pick.setOrigin({pb.size.x / 2.f, 0.f});
        pick.setPosition({float(WIN_W) / 2.f, 240.f});
        window.draw(pick);

        Color cols[4] = {Color::RED, Color::BLUE, Color::GREEN, Color::YELLOW};
        const char *nms[4] = {"Red", "Blue", "Green", "Yellow"};
        float cx = 310.f, cy = 280.f, bw = 110.f, bh = 110.f, bgap = 15.f;

        for (int i = 0; i < 4; i++)
        {
            sf::RectangleShape cb({bw, bh});
            cb.setPosition({cx + i * (bw + bgap), cy});
            cb.setFillColor(toSFColor(cols[i]));
            cb.setOutlineColor(sf::Color::White);
            cb.setOutlineThickness(2.f);
            window.draw(cb);

            sf::Text nl(font, nms[i], 17);
            nl.setFillColor(sf::Color::White);
            sf::FloatRect nb = nl.getLocalBounds();
            nl.setOrigin({nb.size.x / 2.f, nb.size.y / 2.f});
            nl.setPosition({cx + i * (bw + bgap) + bw / 2.f, cy + bh / 2.f});
            window.draw(nl);
        }
    }

    if (phase == TurnPhase::Play_Choosen_Card)
    {
        std::string hint = "Play a  " + colorToString(gm->getPendingWildColor()) + "  card  (or any Wild)";
        sf::Text info(font, hint, 20);
        info.setFillColor(toSFColor(gm->getPendingWildColor()));
        info.setStyle(sf::Text::Bold);
        sf::FloatRect ib = info.getLocalBounds();
        info.setOrigin({ib.size.x / 2.f, 0.f});
        info.setPosition({float(WIN_W) / 2.f, 450.f});
        window.draw(info);
    }

    if (showMsg)
    {
        if (msgClock.getElapsedTime().asSeconds() >= 2.f)
            showMsg = false;
        else
            drawButton(window, font, "Unplayable card! Turn passed.", 433, 550, 500, 52, sf::Color(20, 20, 60));
    }

    if (unoOwnerIdx == currentIdx && !unoAcknowledged && phase == TurnPhase::CHOOSE_ACTION && cur->getHandSize() == 1)
        drawPillButton(window, font, "Call UNO!", UNO_BUTTON_RECT.position.x, UNO_BUTTON_RECT.position.y,
                       UNO_BUTTON_RECT.size.x, UNO_BUTTON_RECT.size.y,
                       sf::Color(212, 168, 35), 22);
}

Screen screen = Screen::MENU;
bool showMsg = false;
sf::Clock msgClock;
int unoOwnerIdx = -1;
bool unoAcknowledged = false;

int main()
{
    sf::RenderWindow window(sf::VideoMode({WIN_W, WIN_H}), "UNO-inspired Card Game", sf::Style::Default);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("assets\\LiberationSans-BoldItalic.ttf"))
        return -1;

    // sf::Music bgMusic;
    // if (!bgMusic.openFromFile("assets\\Let'sPlay.mp3"))
    //     return -1;
    // bgMusic.setLooping(true);
    // bgMusic.setVolume(100.f);
    // bgMusic.play();

    GameManager *gm = GameManager::getInstance();
    gm->resetGame();

    std::vector<std::string> setupNames(4);
    setupNames[0] = "Tom";
    setupNames[1] = "Jerry";
    int setupPlayerCount = 2;
    bool setupCountChosen = true;
    int activeSetupField = 0;

    while (window.isOpen())
    {
        while (auto evOpt = window.pollEvent())
        {
            if (evOpt->is<sf::Event::Closed>())
            {
                window.close();
                break;
            }

            if (auto *te = evOpt->getIf<sf::Event::TextEntered>())
            {
                if (screen == Screen::SETUP && setupCountChosen)
                {
                    char32_t ch = te->unicode;
                    if (ch == 8)
                    {
                        if (!setupNames[activeSetupField].empty())
                            setupNames[activeSetupField].pop_back();
                    }
                    else if (ch >= 32 && ch < 127)
                    {
                        setupNames[activeSetupField].push_back((char)ch);
                    }
                }
                continue;
            }

            if (auto *kp = evOpt->getIf<sf::Event::KeyPressed>())
            {
                if (screen == Screen::SETUP && setupCountChosen && kp->code == sf::Keyboard::Key::Tab)
                    activeSetupField = (activeSetupField + 1) % setupPlayerCount;
                continue;
            }

            auto *mp = evOpt->getIf<sf::Event::MouseButtonPressed>();
            if (!mp || mp->button != sf::Mouse::Button::Left)
                continue;

            sf::Vector2f mouse = window.mapPixelToCoords(sf::Vector2i(mp->position.x, mp->position.y));

            if (screen == Screen::MENU)
            {
                float bx = (WIN_W - 340.f) / 2.f;
                if (sf::FloatRect({bx, 290}, {340, 56}).contains(mouse))
                {
                    screen = Screen::SETUP;
                    setupCountChosen = false;
                    setupPlayerCount = 2;
                    activeSetupField = 0;
                    setupNames.assign(4, std::string());
                }
                else if (sf::FloatRect({bx, 368}, {340, 56}).contains(mouse))
                    screen = Screen::RULES;
                else if (sf::FloatRect({bx, 446}, {340, 56}).contains(mouse))
                    window.close();
            }
            else if (screen == Screen::SETUP)
            {
                const float countW = 120.f;
                const float countH = 60.f;
                const float countGap = 25.f;
                float countX = (float(WIN_W) - (3.f * countW + 2.f * countGap)) / 2.f;
                float countY = 205.f;

                for (int i = 0; i < 3; i++)
                {
                    sf::FloatRect r({countX + i * (countW + countGap), countY}, {countW, countH});
                    if (r.contains(mouse))
                    {
                        setupPlayerCount = 2 + i;
                        setupCountChosen = true;
                        activeSetupField = 0;
                        break;
                    }
                }

                if (setupCountChosen)
                {
                    const float nameX = 370.f;
                    const float nameY = 320.f;
                    const float nameW = 626.f;
                    const float nameH = 44.f;
                    const float nameGap = 14.f;

                    for (int i = 0; i < setupPlayerCount; i++)
                    {
                        sf::FloatRect r({nameX, nameY + i * (nameH + nameGap)}, {nameW, nameH});
                        if (r.contains(mouse))
                        {
                            activeSetupField = i;
                            break;
                        }
                    }

                    sf::FloatRect startRect({(WIN_W - 260.f) / 2.f, 670.f}, {260.f, 52.f});
                    if (startRect.contains(mouse))
                    {
                        bool valid = true;
                        std::vector<std::string> names;
                        for (int i = 0; i < setupPlayerCount; i++)
                        {
                            if (setupNames[i].empty())
                                valid = false;
                            names.push_back(setupNames[i]);
                        }

                        if (valid)
                        {
                            gm->resetGame(names);
                            unoOwnerIdx = -1;
                            unoAcknowledged = false;
                            showMsg = false;
                            screen = Screen::GAME;
                        }
                    }
                }
            }
            else if (screen == Screen::RULES)
            {
                if (sf::FloatRect({583, 754}, {200, 46}).contains(mouse))
                    screen = Screen::MENU;
            }
            else if (screen == Screen::GAME)
            {
                TurnPhase phase = gm->getTurnPhase();
                Player *cur = gm->getCurrentPlayer();
                int currentIdx = gm->getCurrentPlayerIdx();
                bool unoRequired = (phase == TurnPhase::CHOOSE_ACTION && cur && cur->getHandSize() == 1 && unoOwnerIdx == currentIdx && !unoAcknowledged);

                if (phase == TurnPhase::GAME_OVER)
                {
                    const float panelY = (float(WIN_H) - 280.f) / 2.f;
                    const float btnW = 280.f, btnH = 52.f;
                    const float btnX = float(WIN_W) / 2.f - btnW / 2.f;
                    const float btnY = panelY + 205.f;
                    if (sf::FloatRect({btnX, btnY}, {btnW, btnH}).contains(mouse))
                    {
                        setupPlayerCount = gm->getPlayerCount();
                        setupCountChosen = true;
                        activeSetupField = 0;
                        for (int i = 0; i < setupPlayerCount; i++)
                        {
                            Player *p = gm->getPlayer(i);
                            setupNames[i] = p ? p->getName() : std::string();
                        }
                        if (setupPlayerCount < 4)
                            for (int i = setupPlayerCount; i < 4; i++) setupNames[i].clear();
                        screen = Screen::SETUP;
                    }
                    if (sf::FloatRect({btnX, btnY + 60.f}, {btnW, btnH}).contains(mouse))
                        window.close();
                    continue;
                }

                if (unoRequired)
                {
                    if (UNO_BUTTON_RECT.contains(mouse))
                    {
                        unoAcknowledged = true;
                        continue;
                    }

                    gm->addPenaltyCards(currentIdx, 6);
                    unoOwnerIdx = -1;
                    unoAcknowledged = false;
                    gm->advanceTurn();
                    continue;
                }

                if (phase == TurnPhase::CHOOSE_ACTION && BTN_DRAW_CARD_RECT.contains(mouse))
                {
                    gm->drawCard();
                    if (gm->getLastDrawnCard() == nullptr)
                    {
                        showMsg = true;
                        msgClock.restart();
                    }
                    continue;
                }

                if (phase == TurnPhase::AWAITING_DRAW_DECISION)
                {
                    if (BTN_PLAY_IT_RECT.contains(mouse))
                        gm->playDrawnCard();
                    else if (BTN_KEEP_IT_RECT.contains(mouse))
                        gm->keepDrawnCard();
                    continue;
                }

                if (phase == TurnPhase::CHOOSE_COLOR)
                {
                    Color cols[4] = {Color::RED, Color::BLUE, Color::GREEN, Color::YELLOW};
                    float cx = 310.f, cy = 280.f, bw = 110.f, bh = 110.f, bgap = 15.f;
                    for (int i = 0; i < 4; i++)
                    {
                        if (sf::FloatRect({cx + i * (bw + bgap), cy}, {bw, bh}).contains(mouse))
                        {
                            gm->setChosenColor(cols[i]);
                            if (!gm->hasCardsOfColor(cols[i]))
                                gm->skipWildFollowUp();
                            break;
                        }
                    }
                    continue;
                }

                if (phase == TurnPhase::CHOOSE_ACTION || phase == TurnPhase::Play_Choosen_Card)
                {
                    HandLayout layout = computeHandLayout(cur->getHandSize());
                    for (int i = 0; i < cur->getHandSize(); i++)
                    {
                        if (sf::FloatRect({layout.startX + i * (layout.cw + layout.gap), layout.cardY}, {layout.cw, 160.f}).contains(mouse))
                        {
                            if (phase == TurnPhase::CHOOSE_ACTION)
                                gm->tryPlayCard(i);
                            else
                                gm->tryPlayWildFollowUp(i);
                            break;
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(54, 69, 79));

        if (screen == Screen::MENU)
            drawMenuScreen(window, font);
        else if (screen == Screen::RULES)
            drawRulesScreen(window, font);
        else if (screen == Screen::SETUP)
            drawSetupScreen(window, font, setupCountChosen, setupPlayerCount, activeSetupField, setupNames);
        else if (screen == Screen::GAME)
            drawGameScreen(window, font, gm, showMsg, msgClock, unoAcknowledged, unoOwnerIdx);

        window.display();
    }

    return 0;
}