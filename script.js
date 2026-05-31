import { dhhQuestions } from "./data/dhhQuestions.js";

const startScreen = document.getElementById("startScreen");
const modeScreen = document.getElementById("modeScreen");
const quizScreen = document.getElementById("quizScreen");
const gameOverScreen = document.getElementById("gameOverScreen");
const winScreen = document.getElementById("winScreen");

const startBtn = document.getElementById("startBtn");
const restartBtn = document.getElementById("restartBtn");
const playAgainBtn = document.getElementById("playAgainBtn");
const backBtn = document.getElementById("backBtn");

const questionText = document.getElementById("question");
const answerButtons = document.querySelectorAll(".answerBtn");
const levelText = document.getElementById("levelText");
const finalLevel = document.getElementById("finalLevel");

const modeButtons = document.querySelectorAll(".modeBtn");

let selectedMode = "";
let questions = [];
let currentLevel = 0;


/* START BUTTON */
startBtn.addEventListener("click", () => {

  startScreen.classList.remove("active");
  modeScreen.classList.add("active");

});


/* BACK BUTTON */
if(backBtn){

  backBtn.addEventListener("click", () => {

    modeScreen.classList.remove("active");
    startScreen.classList.add("active");

  });

}


/* MODE SELECTION */
modeButtons.forEach(button => {

  button.addEventListener("click", () => {

    selectedMode = button.dataset.mode;

    modeScreen.classList.remove("active");

    startGame();

  });

});


/* RESTART BUTTONS */
restartBtn.addEventListener("click", restartGame);
playAgainBtn.addEventListener("click", restartGame);


/* START GAME */
async function startGame(){

  currentLevel = 0;

  if(selectedMode === "dhh"){

    questions =
      shuffleArray([...dhhQuestions]).slice(0, 10);

    quizScreen.classList.add("active");

    showQuestion();

    return;

  }

  try{

    const response = await fetch(
      "https://opentdb.com/api.php?amount=10&category=17&difficulty=easy&type=multiple"
    );

    const data = await response.json();

    questions = data.results;

    quizScreen.classList.add("active");

    showQuestion();

  }
  catch(error){

    alert("Failed to load science questions.");

    console.log(error);

    modeScreen.classList.add("active");

  }

}


/* SHOW QUESTION */
function showQuestion(){

  levelText.innerText =
    `Level ${currentLevel + 1} / 10`;


  /* DHH MODE */
  if(selectedMode === "dhh"){

    const currentQuestion =
      questions[currentLevel];

    questionText.innerText =
      currentQuestion.question;

    const options =
      shuffleArray([...currentQuestion.options]);

    answerButtons.forEach((button, index) => {

      button.style.display =
        options[index] ? "block" : "none";

      button.innerText =
        options[index] || "";

      button.onclick = () => {

        checkAnswer(options[index]);

      };

    });

    return;

  }


  /* SCIENCE MODE */
  const currentQuestion =
    questions[currentLevel];

  questionText.innerHTML =
    decodeHTML(currentQuestion.question);

  const answers = [

    ...currentQuestion.incorrect_answers,
    currentQuestion.correct_answer

  ];

  shuffleArray(answers);

  answerButtons.forEach((button, index) => {

    button.style.display = "block";

    button.innerHTML =
      decodeHTML(answers[index]);

    button.onclick = () => {

      checkAnswer(answers[index]);

    };

  });

}


/* CHECK ANSWER */
function checkAnswer(selectedAnswer){

  const correctAnswer =
    selectedMode === "dhh"
      ? questions[currentLevel].answer
      : questions[currentLevel].correct_answer;


  if(selectedAnswer === correctAnswer){

    currentLevel++;

    if(currentLevel >= 10){

      quizScreen.classList.remove("active");

      winScreen.classList.add("active");

    }
    else{

      showQuestion();

    }

  }
  else{

    quizScreen.classList.remove("active");

    finalLevel.innerText =
      currentLevel + 1;

    gameOverScreen.classList.add("active");

  }

}


/* RESTART */
function restartGame(){

  quizScreen.classList.remove("active");
  gameOverScreen.classList.remove("active");
  winScreen.classList.remove("active");
  modeScreen.classList.remove("active");

  startScreen.classList.add("active");

  selectedMode = "";
  currentLevel = 0;
  questions = [];

}


/* SHUFFLE */
function shuffleArray(array){

  for(let i = array.length - 1; i > 0; i--){

    const j =
      Math.floor(Math.random() * (i + 1));

    [array[i], array[j]] =
      [array[j], array[i]];

  }

  return array;

}


/* FIX HTML SYMBOLS */
function decodeHTML(text){

  const txt =
    document.createElement("textarea");

  txt.innerHTML = text;

  return txt.value;

}