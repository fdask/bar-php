<?php
class Game {
    private $term;
    private $bar;
    private $ticks;
    private $doubleSpeed;

    public function __construct() {
        $this->term = `stty -g`;
		system("stty -icanon -echo");

        echo "\n";

        $this->bar = new Bar(Config::$defaultBarLength, Config::$defaultTickInterval);
        $this->ticks = 0;
        $this->doubleSpeed = false;

        $this->gameLoop();
    }

    public function gameLoop() {
        $this->draw();
        
        stream_set_blocking(STDIN, false); // Do not wait

        // START OF INFINITE LOOP
        while (1) {
            $c = Config::waitForInput();

            switch ($c) {
                case ' ':
                    // switch direction
                    $this->bar->switchDir();

                    // get the last score
                    $this->bar->setLastScore($this->getScore());

                    break;
                case Config::keypressLeftArrow():
                    // go left if we're going right already
                    if ($this->bar->getDir() == Bar::$dirRight) {
                        $this->bar->switchDir();
                    }

                    break;
                case Config::keypressRightArrow():
                    // go right if we're going left already
                    if ($this->bar->getDir() == Bar::$dirLeft) {
                        $this->bar->switchDir();
                    }

                    break;
                case 'c':
                    // clear out the marks
                    $this->bar->clearMarks();

                    break;
                case 'd':
                    // doubleTime!
                    $this->doubleSpeed = true;

                    break;
                case 'm':
                    // mark
                    $this->bar->addMark();
                    $this->bar->switchDir();

                    break;
                case 'q':
                    // quit the game
                    $this->gameOver();

                    // just in case
                    exit;
                default:
                    break;
            }

            $this->ticks++;

            $speed = $this->bar->getSpeed();

            if ($this->doubleSpeed) {
                $speed = floor($speed / 2);
            }

            if ($this->ticks >= $speed) {
                $this->ticks = 0;
                $this->bar->tick();

                // reset doublespeed if we hit an edge
                if ($this->bar->onEdge()) {
                    $this->doubleSpeed = false;
                }

                $this->draw();
            }
        }
        // END OF INFINITE LOOP
    }

    public function gameOver() {
        echo "GAME OVER\n\n";

        // quit the game
		system("stty " . $this->term);

		exit;
    }

    /** 
     * calculates a "score" as the percentage distance from 0 
     * 
     * utilizes the ticks between drawing to determine distance
     * travelled within a characters bounds
     */
    public function getScore() {
        $x = $this->bar->getX();
        $width = $this->bar->getWidth();

        $segment = 1 / $width;

        $ticks = $this->ticks;
        $tickInterval = $this->bar->getSpeed();

        $percent = $ticks / $tickInterval;
        
        $percentDifference = $segment * ($ticks / $tickInterval);

        return round(($percentDifference + ($segment * $x)) * 100, 2);
    }

    public function draw() {
        Config::draw($this->bar->draw());
    }
}
