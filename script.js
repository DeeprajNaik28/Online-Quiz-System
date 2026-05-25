const startScreen = document.getElementById("startScreen");
const quizScreen = document.getElementById("quizScreen");
const gameOverScreen = document.getElementById("gameOverScreen");
const winScreen = document.getElementById("winScreen");

const startBtn = document.getElementById("startBtn");
const restartBtn = document.getElementById("restartBtn");
const playAgainBtn = document.getElementById("playAgainBtn");

const questionText = document.getElementById("question");
const answerButtons = document.querySelectorAll(".answerBtn");
const levelText = document.getElementById("levelText");
const finalLevel = document.getElementById("finalLevel");

let questions = [];
let currentLevel = 0;


/* START GAME */
startBtn.addEventListener("click", startGame);
restartBtn.addEventListener("click", restartGame);
playAgainBtn.addEventListener("click", restartGame);


/* FETCH QUESTIONS */
async function startGame(){

  startBtn.innerText = "Loading...";

  try{

    const response = await fetch(
      "https://opentdb.com/api.php?amount=10&category=17&type=multiple"
    );

    const data = await response.json();

    questions = data.results;

    currentLevel = 0;

    startScreen.classList.remove("active");
    gameOverScreen.classList.remove("active");
    winScreen.classList.remove("active");

    quizScreen.classList.add("active");

    showQuestion();

  }
  catch(error){

    alert("Failed to load questions.");

    startBtn.innerText = "Start Game";

    console.log(error);

  }

}


/* SHOW QUESTION */
function showQuestion(){

  const currentQuestion = questions[currentLevel];

  levelText.innerText = `Level ${currentLevel + 1} / 10`;

  questionText.innerHTML = decodeHTML(currentQuestion.question);


  // MIX ANSWERS
  const answers = [
    ...currentQuestion.incorrect_answers,
    currentQuestion.correct_answer
  ];

  shuffleArray(answers);


  // DISPLAY ANSWERS
  answerButtons.forEach((button, index) => {

    button.innerHTML = decodeHTML(answers[index]);

    button.onclick = () => {

      checkAnswer(answers[index]);

    };

  });

}


/* CHECK ANSWER */
function checkAnswer(selectedAnswer){

  const correctAnswer = questions[currentLevel].correct_answer;

  if(selectedAnswer === correctAnswer){

    currentLevel++;

    // PLAYER WON
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

    gameOverScreen.classList.add("active");

    finalLevel.innerText = currentLevel + 1;

  }

}


/* RESTART GAME */
function restartGame(){

  gameOverScreen.classList.remove("active");
  winScreen.classList.remove("active");

  startBtn.innerText = "Start Game";

  startScreen.classList.add("active");

}


/* SHUFFLE ANSWERS */
function shuffleArray(array){

  for(let i = array.length - 1; i > 0; i--){

    const j = Math.floor(Math.random() * (i + 1));

    [array[i], array[j]] = [array[j], array[i]];

  }

}


/* FIX HTML SYMBOLS */
function decodeHTML(text){

  const txt = document.createElement("textarea");

  txt.innerHTML = text;

  return txt.value;

}