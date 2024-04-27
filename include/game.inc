<?php
class Game {
    private $term;
    private $bar;
    private $bars;
    private $ticks;
    private $lastScore;
    private $doubleSpeed;
    private $targetBar;

    public function __construct() {
        $this->term = `stty -g`;
		system("stty -icanon -echo");

        echo "\n";
       
        // populate the game!
        $width = rand(Config::$minBarWidth, Config::$maxBarWidth);
        $speed = rand(Config::$minBarSpeed, Config::$maxBarSpeed);

        $b = new Bar($width, $speed);
        $this->bars[] = $b;

        // initialize some settings
        $this->targetBar = 0;
        $this->lastScore = null;
        $this->bar = 0; 
        $this->ticks = 0;
        $this->doubleSpeed = false;

        $this->addBar();

        $this->addTarget();

        // enter the main loop
        $this->gameLoop();
    }

    public function addTarget($bar = null, $x = null) {
        if (is_null($bar)) {
            $bar = $this->bar;
        }

        if (is_null($x)) {
            $x = $this->bars[$bar]->getUnoccupiedX();
        }

        $t = new Target($x);

        $this->targetBar = $bar;
        $this->bars[$bar]->addContent($x, $t);
    }

    public function addBar() {
        $width = rand(Config::$minBarWidth, Config::$maxBarWidth);
        $speed = rand(Config::$minBarSpeed, Config::$maxBarSpeed);

        $bar = new Bar($width, $speed);
        $destX = rand(0, $width - 1);
        $origX = $this->bars[$this->bar]->getUnoccupiedX();

        // add a door
        $orig = count($this->bars) - 1;
        $dest = count($this->bars);

        $this->bars[] = $bar;

        $this->addDoor($orig, $dest, $origX, $destX);
    }

    public function setBar($b) {
        $this->bar = $b;

        if ($b == count($this->bars) - 1) {
            $this->addBar();
        }
    }

    public function addDoor($origBar, $destBar, $origX, $destX) {
        if (isset($this->bars[$origBar]) && isset($this->bars[$destBar])) {
            $doorA = new Door($origBar, $origX, $destBar, $destX, Door::$dirUp);
            $doorB = new Door($destBar, $destX, $origBar, $origX, Door::$dirDown);
            $this->bars[$origBar]->addContent($origX, $doorA);
            $this->bars[$destBar]->addContent($destX, $doorB);
        } 
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
                    $this->bars[$this->bar]->switchDir();

                    // get the last score
                    $this->lastScore = $this->getScore();

                    // if we're on a target, remove it
                    $curX = $this->bars[$this->bar]->getX();
                    $targets = $this->bars[$this->bar]->getTargets();

                    if (in_array($curX, $targets)) {
                        $this->bars[$this->bar]->clearTargets();

                        // add a new target elsewheres
                        $randBar = rand(0, count($this->bars) - 1);
                        $randX = $this->bars[$randBar]->getUnoccupiedX();

                        $this->addTarget($randBar, $randX);
                    }

                    break;
                case Config::keypressUpArrow():
                case Config::keypressDownArrow():
                    // check to see if we hit a doorway
                    if ($this->bars[$this->bar]->onDoor()) {
                        $door = $this->bars[$this->bar]->getContents();
                        $dest = $door->getDest();
                        $destX = $door->getDestX();
                        $dir = $door->getDir();

                        if (
                            ($dir == Door::$dirUp && $c == Config::keypressUpArrow()) ||
                            ($dir == Door::$dirDown && $c == Config::keypressDownArrow())
                        ) {
                            // go through the door
                            $oldbar = $this->bar;
                            $olddir = $this->bars[$this->bar]->getDir();

                            // go to the $dest bar
                            $this->setBar($dest);

                            // we need to figure out whee we're coming out
                            $this->bars[$this->bar]->setX($destX);
                            $this->bars[$this->bar]->setDir($olddir);
                        }
                    } else {
                        echo "MISSED THE DOOR!\n";
                    }
                
                    break;
                case Config::keypressLeftArrow():
                    // go left if we're going right already
                    if ($this->bars[$this->bar]->getDir() == Bar::$dirRight) {
                        $this->bars[$this->bar]->switchDir();
                    }

                    break;
                case Config::keypressRightArrow():
                    // go right if we're going left already
                    if ($this->bars[$this->bar]->getDir() == Bar::$dirLeft) {
                        $this->bars[$this->bar]->switchDir();
                    }

                    break;
                case 'c':
                    // clear out the marks
                    
                    break;
                case Config::keypressShiftRightArrow():
                    if ($this->bars[$this->bar]->getDir() == Bar::$dirRight) {
                        $this->doubleSpeed = true;
                    } else {
                        $this->bars[$this->bar]->switchDir();
                    }

                    break;
                case Config::keypressShiftLeftArrow():
                    if ($this->bars[$this->bar]->getDir() == Bar::$dirLeft) {
                        $this->doubleSpeed = true;
                    } else {
                        $this->bars[$this->bar]->switchDir();
                    }

                    break;
                case 'd':
                    // doubleTime!
                    $this->doubleSpeed = true;

                    break;
                case 'm':
                    // mark
                    $this->bars[$this->bar]->addMark();
                    $this->bars[$this->bar]->switchDir();

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

            $x = $this->bars[$this->bar]->getX();
            $speed = $this->bars[$this->bar]->getSpeed();

            if ($this->doubleSpeed) {
                $speed = floor($speed / 2);
            }

            if ($this->ticks >= $speed) {
                $this->ticks = 0;
                $this->bars[$this->bar]->tick();

                // reset doublespeed if we hit an edge
                if ($this->bars[$this->bar]->onEdge()) {
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
        $x = $this->bars[$this->bar]->getX();
        $width = $this->bars[$this->bar]->getWidth();

        $segment = 1 / $width;

        $ticks = $this->ticks;
        $tickInterval = $this->bars[$this->bar]->getSpeed();

        $percent = $ticks / $tickInterval;
        
        $percentDifference = $segment * ($ticks / $tickInterval);

        return round(($percentDifference + ($segment * $x)) * 100, 2);
    }

    public function draw() {
        $d = $this->bars[$this->bar]->draw();

        $d .= " b: " . $this->bar . "/" . count($this->bars) . " (" . $this->targetBar . ")";

        if (!is_null($this->lastScore)) {
            $d .= " [" . $this->lastScore . "%]";
        }

        $d .= "\n";

        Config::draw($d);
    }
}
