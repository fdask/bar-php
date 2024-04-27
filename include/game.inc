<?php
class Game {
    // save terminal settings to restore later
    private $term;

    // what bar # are we currently on
    private $bar;

    // the collection of bars
    private $bars;

    // main game timer
    private $ticks;

    // % from our last shot
    private $lastScore;

    // boolean
    private $doubleSpeed;

    // what bar # is the target on
    private $targetBar;

    // player stats
    private $health;
    private $shots;
    private $kills;
    private $misses;

    // the level data object
    private $level;

    // bumpers
    private $leftBumper;
    private $rightBumper;

    public static $bumperDmg = 'DMG';
    public static $bumperNoop = 'NOOP';
    public static $bumperBase = 'BASE';

    public function __construct() {
        $this->term = `stty -g`;
		system("stty -icanon -echo");

        echo "\n";
       
        $this->level = new Level();

        // populate the initial bar!
        $width = rand(Config::$minBarWidth, Config::$maxBarWidth);
        $speed = rand($this->level->minBarSpeed, $this->level->maxBarSpeed);

        $b = new Bar($width, $speed);
        $this->bars[] = $b;

        // populate the initial bumpers
        $this->leftBumper = Game::$bumperBase;
        $this->rightBumper = Game::$bumperNoop;

        // initialize some settings
        $this->misses = 0;
        $this->kills = 0;
        $this->targetBar = 0;
        $this->health = Config::$defaultHealth;
        $this->shots = Config::$defaultShots;
        $this->lastScore = null;
        $this->bar = 0; 
        $this->ticks = 0;
        $this->doubleSpeed = false;

        // add the initial bar
        $this->addBar();

        // add the initial target
        if ($this->level->targetSize > 1) {
            $this->addWideTarget($this->level->targetSize);
        } else {
            $this->addTarget();    
        }

        // enter the main loop
        $this->gameLoop();
    }

    public function reset() {
        $this->bar = 0;
        $this->bars = array();

        $width = rand(Config::$minBarWidth, Config::$maxBarWidth);
        $speed = rand($this->level->minBarSpeed, $this->level->maxBarSpeed);

        $b = new Bar($width, $speed);
        $this->bars[] = $b;
        
        $this->addBar();

        // add the initial target
        if ($this->level->targetSize > 1) {
            $this->addWideTarget($this->level->targetSize);
        } else {
            $this->addTarget();    
        }
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

    public function addWideTarget($width, $bar = null) {
        if (is_null($bar)) {
            $bar = $this->bar;
        }

        $xs = $this->bars[$bar]->getUnoccupiedXStretch($width);

        foreach ($xs as $x) {
            $this->addTarget($bar, $x);
        }
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

        // if we're at the upper limit, add a new bar
        if ($b == count($this->bars) - 1) {
            $this->addBar();
        }

        // set the left and right bumper
        if ($this->bar == 0) {
            $this->leftBumper = Game::$bumperBase;
        } else {
            if (rand(0, 1)) {
                $this->leftBumper = Game::$bumperDmg;
            } else {
                $this->leftBumper = Game::$bumperNoop;
            }
        }

        if (rand(0, 1)) {
            $this->rightBumper = Game::$bumperDmg;
        } else {
            $this->rightBumper = Game::$bumperNoop;
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
                    if ($this->shots > 0) {
                        // switch direction
                        $this->bars[$this->bar]->switchDir();

                        // get the last score
                        $this->lastScore = $this->getScore();

                        // if we're on a target, remove it
                        $curX = $this->bars[$this->bar]->getX();
                        $targets = $this->bars[$this->bar]->getTargets();

                        if (in_array($curX, $targets)) {
                            // HIT
                            $this->kills++;
                            $this->level->targetProgress++;

                            // check to see if this clears the level
                            if ($this->level->targetProgress >= $this->level->targetCount) {
                                // level up
                                $this->level->levelUp();

                                // reset the level
                                $this->reset();
                            }

                            $this->bars[$this->bar]->clearTargets();

                            // add a new target elsewheres
                            $randBar = rand(0, count($this->bars) - 1);
                            
                            if ($this->level->targetSize > 1) {
                                $xs = $this->bars[$randBar]->getUnoccupiedXStretch($this->level->targetSize);

                                foreach ($xs as $x) {
                                    $this->addTarget($randBar, $x);
                                }
                            } else {
                                $randX = $this->bars[$randBar]->getUnoccupiedX();
                            
                                $this->addTarget($randBar, $randX);
                            }
                        } else {
                            // MISS
                            $this->misses++;
                        }

                        $this->shots--;
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

                // if we hit an edge
                if ($this->bars[$this->bar]->onEdge()) {
                    $this->doubleSpeed = false;

                    // check what type of bumper we've hit
                    if ($this->bars[$this->bar]->getX() == 0) {
                        if ($this->leftBumper == Game::$bumperDmg) {
                            $this->health--;
                        } else if ($this->leftBumper == Game::$bumperBase) {
                            $this->shots = Config::$defaultShots;
                        }
                    } else {
                        if ($this->rightBumper == Game::$bumperDmg) {
                            $this->health--;
                        }
                    }

                    if ($this->health <= 0) {
                        echo "OUT OF HEALTH\n";

                        $this->gameOver();
                    }
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
        switch ($this->leftBumper) {
            case Game::$bumperDmg:
                $d = Config::$bumperDmgChar;
                break;
            case Game::$bumperBase:
                $d = Config::$bumperBaseChar;
                break;
            case Game::$bumperNoop:
                $d = Config::$bumperNoopChar;
                break;
            default:
                // do nothing
        }

        $d .= $this->bars[$this->bar]->draw();

        switch ($this->rightBumper) {
            case Game::$bumperDmg:
                $d .= Config::$bumperDmgChar;
                break;
            case Game::$bumperBase:
                $d .= Config::$bumperBaseChar;
                break;
            case Game::$bumperNoop:
                $d .= Config::$bumperNoopChar;
                break;
            default:
                // do nothing
        }

        $d .= " b: " . ($this->bar + 1) . "/" . count($this->bars) . " (" . ($this->targetBar + 1) . ")";
        $d .= " h: " . $this->health . " s: " . $this->shots . " l: " . $this->level->level . " r: " . ($this->level->targetCount - $this->level->targetProgress);
        
        //$d .= " k: " . $this->kills . " m: " . $this->misses;

        if (!is_null($this->lastScore)) {
            $d .= " [" . $this->lastScore . "%]";
        }

        $d .= "\n";

        Config::draw($d);
    }
}
