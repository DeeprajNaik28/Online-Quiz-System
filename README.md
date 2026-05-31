# Quiz Master 🎮🧠

A web-based quiz game built using HTML, CSS, and JavaScript.

Players must answer questions correctly to progress through 10 levels. One wrong answer ends the game.

The game currently supports multiple quiz categories and is designed to be easily expandable with additional question packs and game modes.

---

# Current Quiz Modes

### 🔬 Science Quiz

* Uses the Open Trivia DB API
* Easy difficulty science questions
* Random questions every game

### 🎤 Indian Hip Hop Quiz

* Custom-built DHH question bank
* Questions about artists, songs, albums, collaborations, labels, and DHH culture
* Randomly selects 10 questions from a larger question pool every game

---

# Features

* 10 Level Quiz Gameplay
* Quiz Category Selection Screen
* Science Quiz Mode
* Indian Hip Hop Quiz Mode
* Random Questions Every Game
* Randomized Answer Positions
* Neon Game UI
* Win Screen
* Game Over Screen
* Replay Functionality
* Mobile Responsive Design
* Expandable Question Bank System

---

# Tech Used

* HTML5
* CSS3
* JavaScript (ES6 Modules)
* Open Trivia DB API

---

# Project Structure

```txt
quiz-master/
│
├── index.html
├── style.css
├── script.js
│
└── data/
    └── dhhQuestions.js
```

---

# How To Run Locally

Because this project uses JavaScript modules (`import/export`), opening `index.html` directly using `file://` may cause browser security restrictions.

Recommended:

1. Open the project in VS Code
2. Install the Live Server extension
3. Right-click `index.html`
4. Click **Open with Live Server**

---

# Game Flow

```txt
Home Screen
     ↓
Start Game
     ↓
Select Quiz Type
     ↓
Science Quiz
or
Indian Hip Hop Quiz
     ↓
Answer Questions
     ↓
Correct Answer → Next Level
Wrong Answer → Game Over
     ↓
Complete Level 10 → Victory
```

---

# Future Updates 🚀

### Planned Game Modes

* Timer Mode
* Survival Mode
* Indian GK Quiz
* Bollywood Quiz
* Mixed Quiz Mode

### Planned Features

* Score System
* Time-Based Bonus Points
* Progress Bar
* Sound Effects
* Correct/Wrong Answer Animations
* Difficulty Progression
* High Score System
* Local Storage Save Data
* Leaderboards