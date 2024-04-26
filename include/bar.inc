<?php
class Bar {
    private $x;
    private $width;
    private $dir;
    private $marks;
    private $lastScore;
    private $speed;
    private $doors;

    public static $dirLeft = "LEFT";
    public static $dirRight = "RIGHT";

    public function __construct($width, $speed) {
        $this->x = 0;
        $this->width = $width;
        $this->speed = $speed;
        $this->dir = Bar::$dirRight;
        $this->marks = array();
        $this->doors = array();
    }

    public function addDoor(Door $d) {
        $this->doors[] = $d;
    }

    public function getDoors() {
        return $this->doors;
    }

    // get the X of a door with a specific destination
    public function getDoorX($dest) {
        foreach ($this->doors as $door) {
            if ($door->getDest() == $dest) {
                $x = $door->getX();

                return $x;
            }
        }

        return false;
    }

    public function getDoorDir() {
        foreach ($this->doors as $door) {
            if ($this->x == $door->getX()) {
                return $door->getDir();
            }
        }
    }

    public function getSpeed() {
        return $this->speed;
    }

    public function setSpeed($s) {
        $this->speed = $s;
    }

    public function setLastScore($ls) {
        $this->lastScore = $ls;
    }

    public function clearMarks() {
        $this->marks = array();
    }

    public function addMark() {
        $this->marks[] = $this->x;
    }

    public function getX() {
        return $this->x;
    }

    public function setX($x) {
        $this->x = $x;
    }

    public function switchDir() {
        if ($this->dir == Bar::$dirRight) {
            $this->dir = Bar::$dirLeft;
        } else {
            $this->dir = Bar::$dirRight;
        }
    }

    public function move() {
        if ($this->dir == Bar::$dirRight) {
            $this->moveRight();
        } else {
            $this->moveLeft();
        }
    }

    public function moveLeft() {
        if ($this->x > 0) {
            $this->x--;
        } else {
            $this->dir = Bar::$dirRight;
            $this->x++;
        }
    }

    public function onEdge() {
        if ($this->x == 0 || $this->x == ($this->width - 1)) {
            return true;
        }

        return false;
    }

    public function onDoor() {
        $doors = $this->getDoors();

        foreach ($doors as $door) {
            if ($this->x == $door->getX()) {
                return $door->getDest();
            }
        }

        return false;
    }

    public function moveRight() {
        if ($this->x < $this->width - 1) {
            $this->x++;
        } else {
            $this->dir = Bar::$dirLeft;
            $this->x--;
        }
    }

    public function getDir() {
        return $this->dir;
    }

    public function setDir($d) {
        $this->dir = $d;
    }

    public function getWidth() {
        return $this->width;
    }

    public function tick() {
        $this->move();
    }

    public function draw() {
        $ret = "";

        $gotChar = false;
        $doors = $this->getDoors();
        
        for ($x = 0; $x < $this->width; $x++) {
            $gotChar = false;

            // are we on a door
            if (count($doors)) {
                foreach ($doors as $door) {
                    if ($x == $door->getX()) {
                        $dir = $door->getDir();

                        if ($dir == Door::$dirUp) {
                            //echo "Door going up!\n";
                            $ret .= Config::arrowUp();
                        } else {
                            //echo "Door doing down!\n";
                            $ret .= Config::arrowDown();
                        }

                        $gotChar = true;
                    }
                }
            }
            
            if (!$gotChar) {
                if ($x == $this->x) {
                    $ret .= "|";
                } else if (in_array($x, $this->marks)) {
                    $ret .= "m";
                } else {
                    $ret .= "-";
                }
            }
        }

        if (!is_null($this->lastScore)) {
            $ret .= " [" . $this->lastScore . "%]";
        }

        return $ret;
    }
}
