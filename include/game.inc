<?php
class Game {
    private $term;
    private $bar;
    private $bars;
    private $ticks;
    private $doubleSpeed;

    public function __construct() {
        $this->term = `stty -g`;
		system("stty -icanon -echo");

        echo "\n";

        $this->bar = 0; 
        $b = new Bar(Config::$defaultBarLength, Config::$defaultTickInterval);
        $b->addDoor(new Door(3, 1, Door::$dirUp));
        $b->addDoor(new Door(30, 2, Door::$dirDown));

        $c = new Bar(30, 3000);
        $c->addDoor(new Door(25, 0, Door::$dirDown));
        $c->addDoor(new Door(3, 2, Door::$dirUp));

        $d = new Bar(rand(20, 100), rand(300, 3000));
        $d->addDoor(new Door(10, 0, Door::$dirUp));
        $d->addDoor(new Door(20, 1, Door::$dirDown));
        $this->bars[] = $b;
        $this->bars[] = $c;
        $this->bars[] = $d;
        
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
                    $this->bars[$this->bar]->switchDir();

                    // get the last score
                    $this->bars[$this->bar]->setLastScore($this->getScore());

                    break;
                case Config::keypressUpArrow():
                case Config::keypressDownArrow():
                    // check to see if we hit a doorway
                    $dest = $this->bars[$this->bar]->onDoor();

                    // we're on a door.  change to it!
                    if ($dest !== false) {
                        //echo "ON THE DOOR!\n";
                        
                        $dir = $this->bars[$this->bar]->getDoorDir();

                        //echo "dir is '$dir'\n";

                        if (
                            ($dir == Door::$dirUp && $c == Config::keypressUpArrow()) ||
                            ($dir == Door::$dirDown && $c == Config::keypressDownArrow())
                        ) {
                            $oldbar = $this->bar;
                            $olddir = $this->bars[$this->bar]->getDir();
                            $this->bar = $dest;

                            $newX = $this->bars[$this->bar]->getDoorX($oldbar);
                            //echo "newX is $newX\n";
                            $this->bars[$this->bar]->setX($newX);
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
                    $this->bars[$this->bar]->clearMarks();

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
        
        $d .= " bar: " . $this->bar . "\n";

        Config::draw($d);
    }
}
