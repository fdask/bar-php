<?php
class Bomb {
    private $x;

    public function __construct($x) {
        $this->x = $x;
    }

    public function getX() {
        return $this->x;
    }
}